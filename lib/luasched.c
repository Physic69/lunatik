

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/version.h>
#include <linux/bpf.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <lunatik.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0))
#include <linux/btf.h>
#include <linux/btf_ids.h>
#include <net/xdp.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 7, 0))
__bpf_kfunc_start_defs();
#else
__diag_push();
__diag_ignore_all("-Wmissing-prototypes",
                  "Global kfuncs as their definitions will be in BTF");
#endif
#endif

// bpf_object wrapper
typedef struct {
    struct bpf_object *obj;
} lua_bpf_object;

static int lua_bpf_object_load(lua_State *L) {
    lua_bpf_object *lobj = luaL_checkudata(L, 1, "bpf_object");
    int err = bpf_object__load(lobj->obj);
    if (err)
        return luaL_error(L, "bpf_object__load failed: %d", err);
    return 0;
}

static int lua_bpf_object_get_program(lua_State *L) {
    lua_bpf_object *lobj = luaL_checkudata(L, 1, "bpf_object");
    const char *section = luaL_checkstring(L, 2);

    struct bpf_program *prog = bpf_object__find_program_by_title(lobj->obj, section);
    if (!prog)
        return luaL_error(L, "program '%s' not found", section);

    struct bpf_program **ud = lua_newuserdata(L, sizeof(*ud));
    *ud = prog;

    luaL_getmetatable(L, "bpf_program");
    lua_setmetatable(L, -2);
    return 1;
}

static int lua_bpf_object_gc(lua_State *L) {
    lua_bpf_object *lobj = luaL_checkudata(L, 1, "bpf_object");
    if (lobj->obj)
        bpf_object__close(lobj->obj);
    return 0;
}

// bpf_program wrapper
static int lua_bpf_program_fd(lua_State *L) {
    struct bpf_program *prog = *(struct bpf_program **)luaL_checkudata(L, 1, "bpf_program");
    int fd = bpf_program__fd(prog);
    lua_pushinteger(L, fd);
    return 1;
}

// bpf.open(path)
static int lua_bpf_open(lua_State *L) {
    const char *path = luaL_checkstring(L, 1);

    struct bpf_object *obj = NULL;
    obj = bpf_object__open_file(path, NULL);
    if (!obj)
        return luaL_error(L, "failed to open BPF object from %s", path);

    lua_bpf_object *lobj = lua_newuserdata(L, sizeof(*lobj));
    lobj->obj = obj;

    luaL_getmetatable(L, "bpf_object");
    lua_setmetatable(L, -2);
    return 1;
}


static const luaL_Reg lua_bpf_object_lib[] = {
	{"load", lua_bpf_object_load},
	{"get_prog", lua_bpf_object_get_program},
	{"__gc" ,lua_bpf_object_gc},
	{NULL}
};

static const luaL_Reg lua_bpf_object_mt[] = {
	{"fd", lua_bpf_program_fd}
};


LUNATIK_NEWLIB(sched,  lua_bpf_object_lib, NULL, NULL);

static int __init lua_bpf_object_init(void)
{
	return 0;
}

static void __exit lua_bpf_object_exit(void)
{
}

module_init(lua_bpf_object_init);
module_exit(lua_bpf_object_exit);
