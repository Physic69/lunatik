ifeq ($(ARCH), x86)
	ifdef CONFIG_X86_32
		KLIBC_ARCH := i386
		asflags-y += -D_REGPARM
	else
		KLIBC_ARCH := x86_64
	endif
else
	KLIBC_ARCH := $(ARCH)
endif

KLIBC_USR := /klibc/usr
KLIBC_INC := $(KLIBC_USR)/include/arch/$(KLIBC_ARCH)

LUNATIK_FLAGS := -D_LUNATIK -D_KERNEL -I${PWD}$(KLIBC_INC)

asflags-y += $(LUNATIK_FLAGS)
ccflags-y += $(LUNATIK_FLAGS) -DLUNATIK_RUNTIME=$(CONFIG_LUNATIK_RUNTIME) \
	-Wimplicit-fallthrough=0 -I$(src) -I${PWD} -I${PWD}/include -I${PWD}/lua

obj-$(CONFIG_LUNATIK) += lunatik.o

lunatik-objs += lua/lapi.o lua/lcode.o lua/lctype.o lua/ldebug.o lua/ldo.o \
	lua/ldump.o lua/lfunc.o lua/lgc.o lua/llex.o lua/lmem.o \
	lua/lobject.o lua/lopcodes.o lua/lparser.o lua/lstate.o \
	lua/lstring.o lua/ltable.o lua/ltm.o \
	lua/lundump.o lua/lvm.o lua/lzio.o lua/lauxlib.o lua/lbaselib.o \
	lua/lcorolib.o lua/ldblib.o lua/lstrlib.o \
	lua/ltablib.o lua/lutf8lib.o lua/lmathlib.o lua/linit.o \
	lua/loadlib.o $(KLIBC_USR)/klibc/arch/$(KLIBC_ARCH)/setjmp.o \
	lunatik_aux.o lunatik_obj.o lunatik_core.o

obj-$(CONFIG_LUNATIK_RUN) += lunatik_run.o

obj-$(CONFIG_LUNATIK_DEVICE) += lib/luadevice.o
obj-$(CONFIG_LUNATIK_LINUX) += lib/lualinux.o
obj-$(CONFIG_LUNATIK_NOTIFIER) += lib/luanotifier.o
obj-$(CONFIG_LUNATIK_SOCKET) += lib/luasocket.o
obj-$(CONFIG_LUNATIK_RCU) += lib/luarcu.o
obj-$(CONFIG_LUNATIK_THREAD) += lib/luathread.o
obj-$(CONFIG_LUNATIK_FIB) += lib/luafib.o
obj-$(CONFIG_LUNATIK_DATA) += lib/luadata.o
obj-$(CONFIG_LUNATIK_PROBE) += lib/luaprobe.o
obj-$(CONFIG_LUNATIK_SYSCALL) += lib/luasyscall.o
obj-$(CONFIG_LUNATIK_XDP) += lib/luaxdp.o
obj-$(CONFIG_LUNATIK_FIFO) += lib/luafifo.o
obj-$(CONFIG_LUNATIK_XTABLE) += lib/luaxtable.o
obj-$(CONFIG_LUNATIK_NETFILTER) += lib/luanetfilter.o
obj-$(CONFIG_LUNATIK_COMPLETION) += lib/luacompletion.o
obj-$(CONFIG_LUNATIK_CRYPTO_SHASH) += lib/luacrypto_shash.o
obj-$(CONFIG_LUNATIK_CRYPTO_SKCIPHER) += lib/luacrypto_skcipher.o
obj-$(CONFIG_LUNATIK_CRYPTO_AEAD) += lib/luacrypto_aead.o
obj-$(CONFIG_LUNATIK_CRYPTO_RNG) += lib/luacrypto_rng.o
obj-$(CONFIG_LUNATIK_CRYPTO_COMP) += lib/luacrypto_comp.o

