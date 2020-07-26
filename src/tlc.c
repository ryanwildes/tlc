#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <libtcc.h>

#define TLC_CONTEXT "tlc_context"

typedef struct tlc_context
{
    TCCState *state;
} tlc_context;

static int tlc_new(lua_State *L)
{
    tlc_context *ctx = lua_newuserdata(L, sizeof(tlc_context));
    ctx->state = tcc_new();
    if (!ctx->state) {
        lua_pushliteral(L, "couldn't create new context");
        return lua_error(L);
    }
    luaL_getmetatable(L, TLC_CONTEXT);
    lua_setmetatable(L, -2);
    return 1;
}

static int tlc_delete(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    if (ctx->state == NULL) {
        lua_pushliteral(L, "attempted to close already deleted context");
        return lua_error(L);
    }
    tcc_delete(ctx->state);
    ctx->state = NULL;
    lua_pushboolean(L, 1);
    return 1;
}

static int tlc_setlibpath(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *path = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_set_lib_path(ctx->state, path);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

// static int tlc_seterrorfunc()

static int tlc_setoptions(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *opts = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_set_options(ctx->state, opts);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_addincludepath(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *path = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_add_include_path(ctx->state, path);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_addsysincludepath(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *path = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_add_sysinclude_path(ctx->state, path);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_defsymbol(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *symbol = luaL_checkstring(L, 2);
    const char *value = luaL_tolstring(L, 3, NULL);
    if (ctx->state != NULL) {
        tcc_define_symbol(ctx->state, symbol, value);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_undefsymbol(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *symbol = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_undefine_symbol(ctx->state, symbol);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_addfile(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *file = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        if (tcc_add_file(ctx->state, file) == -1) {
            lua_pushliteral(L, "error adding file to context");
            return lua_error(L);
        }
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_compilestring(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *str = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        if (tcc_compile_string(ctx->state, str) == -1) {
            lua_pushliteral(L, "error compiling string");
            return lua_error(L);
        }
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_setoutputtype(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    int output = luaL_checkinteger(L, 2);
    if (ctx->state != NULL) {
        tcc_set_output_type(ctx->state, output);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

// static int tlc_addlibpath()
// static int tlc_addlib()
// static int tlc_addsym()

static int tlc_outputfile(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    const char *file = luaL_checkstring(L, 2);
    if (ctx->state != NULL) {
        tcc_output_file(ctx->state, file);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int tlc_run(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    if (ctx->state != NULL) {
        int argc = luaL_checkinteger(L, 2);
        char *argv[argc];
        argv[0] = "tcc";
        for (int i = 1; i <= argc; i++) {
            argv[i] = (char *)luaL_checkstring(L, i + 2);
        }
        int result = tcc_run(ctx->state, argc, argv);
        if (result == 0) {
            lua_pushboolean(L, 1);
            return 1;
        }
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

// static int tlc_relocate()
// static int tlc_getsym()

static int tlc_tostring(lua_State *L)
{
    tlc_context *ctx = luaL_checkudata(L, 1, TLC_CONTEXT);
    if (ctx->state == NULL) {
        lua_pushfstring(L, "%s (closed)", TLC_CONTEXT);
    }
    else {
        lua_pushfstring(L, "%s (open): %p", TLC_CONTEXT, &ctx->state);
    }
    return 1;
}

static const luaL_Reg tlc_functions[] =
{
    { "new", tlc_new },
    { NULL, NULL }
};

static const luaL_Reg tlc_methods[] =
{
    { "delete", tlc_delete },
    { "setlibpath", tlc_setlibpath },
    { "setoptions", tlc_setoptions },
    { "addincludepath", tlc_addincludepath },
    { "addsysincludepath", tlc_addsysincludepath },
    { "defsymbol", tlc_defsymbol },
    { "undefsymbol", tlc_undefsymbol },
    { "addfile", tlc_addfile },
    { "compilestring", tlc_compilestring },
    { "setoutputtype", tlc_setoutputtype },
    { "run", tlc_run },
    { "__gc", tlc_delete },
    { "__close", tlc_delete },
    { "__tostring", tlc_tostring },
    { NULL, NULL }
};

int luaopen_tlc(lua_State *L)
{
    luaL_newmetatable(L, TLC_CONTEXT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, tlc_methods, 0);
    luaL_newlib(L, tlc_functions);

    // add output types
    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "memory");
    lua_pushinteger(L, 2);
    lua_setfield(L, -2, "exe");
    lua_pushinteger(L, 3);
    lua_setfield(L, -2, "dll");
    lua_pushinteger(L, 4);
    lua_setfield(L, -2, "obj");
    lua_pushinteger(L, 5);
    lua_setfield(L, -2, "preprocess");
    lua_setfield(L, -2, "output");
    return 1;
}
