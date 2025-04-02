--
-- SPDX-FileCopyrightText: (c) 2024 Ring Zero Desenvolvimento de Software LTDA
-- SPDX-License-Identifier: MIT OR GPL-2.0-only
--

local fifo       = require("fifo")
local completion = require("completion")

local lunatik    = require("lunatik")
local rcu_table  = require("rcu").table
local new_data   = require("data").new

-- Mailbox using fifo and completion

local mailbox = {}
local MailBox = {}
MailBox.__index = MailBox

local function new(q, e, allowed, forbidden)
	-- Setting e to false makes this Mailbox non-blocking,
	-- hence suitable for non-sleepable runtimes.
	local mbox = {}
	if type(q) == 'userdata' then
		mbox.queue, mbox.event = q, e
	else
		mbox.queue = fifo.new(q)
		if e ~= false then
			mbox.event = completion.new()
		end
	end
	mbox[forbidden] = function () error(allowed .. "-only mailbox") end
	return setmetatable(mbox, MailBox)
end

function mailbox.inbox(q, e)
	return new(q, e, 'receive', 'send')
end

function mailbox.outbox(q, e)
	return new(q, e, 'send', 'receive')
end

local sizeoft = string.packsize("T")
function MailBox:receive(timeout)
	-- Setting no timeout or timeout = 0 makes this function non-blocking.
	if self.event or not timeout or timeout == 0 then
		local ok, err = self.event:wait(timeout)
		if not ok then error(err) end
	end

	local queue = self.queue
	local header, header_size = queue:pop(sizeoft)

	if header_size == 0 then
		return nil
	elseif header_size < sizeoft then
		error("malformed message")
	end

	return queue:pop(string.unpack("T", header))
end

function MailBox:send(message)
	self.queue:push(string.pack("s", message))
	if self.event then self.event:complete() end
end

-- Msgbox using rcu

local env = lunatik._ENV

local MsgBox = {}
MsgBox.__index = MsgBox

local function new_rcu(name, hsize, allowed, forbidden)
	hsize = hsize or 1024

	local mbox = env[name]
	if not mbox then
		mbox = rcu_table(hsize)
		mbox.last_write = new_data(8)
		mbox.last_read = new_data(8)
		env[name] = mbox
	end
	local self = {
		name = name,
		[forbidden] = function () error(allowed .. "-only mailbox") end
	}
	return setmetatable(self, MsgBox)
end

function MsgBox:send(msg)
	-- We don’t set mbox within the object, to avoid duplicates in race conditions
	local mbox = env[self.name]
	local last_write = mbox.last_write
	local index = 1 + last_write:getnumber(0)

	local msg_data = new_data(#msg)
	msg_data:setstring(0, msg)
	mbox[index] = msg_data

	last_write:setnumber(0, index)
end

function MsgBox:receive()
	local mbox = env[self.name]
	local last_read = mbox.last_read
	local last_write = mbox.last_write
	local index = last_read:getnumber(0)

	if index ~= last_write:getnumber(0) then
		index = index + 1
		local msg_data = mbox[index]

		if msg_data then  -- This should be, but this condition avoids race conditions
			last_read:setnumber(0, index)
			local msg = msg_data:getstring(0)
			mbox[index] = nil
			return msg
		end
	end
end

function mailbox.rcu_inbox(name, hsize)
	return new_rcu(name, hsize, 'receive', 'send')
end

function mailbox.rcu_outbox(name, hsize)
	return new_rcu(name, hsize, 'send', 'receive')
end

return mailbox

