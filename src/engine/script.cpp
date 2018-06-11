#include "script.h"

using namespace script;

std::vector<libs *> libs_vector;

libs::libs() {

}

libs::~libs() {

}

void add_lib( std::string name, void (*func)( lua_State *)) {
    // install load lua
    bool l_loaded = false;
    for( auto const& lib:libs_vector)
        if( lib->name == name)
            l_loaded = true;
    if( !l_loaded) {
        libs *l_lib = new libs;
        l_lib->func = func;
        l_lib->name = name;
        libs_vector.push_back( l_lib);
    }
}

void install_libs( lua_State *state) {
    for( auto const& l_lib:libs_vector)
        l_lib->func( state);
}

void script::error (lua_State *luastate, char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vfprintf( stderr, fmt, argp);

  va_end(argp);
}

script::script::script( const std::string& filename)
{
    p_luastate = luaL_newstate();
    luaL_openlibs( p_luastate);
    if (luaL_loadfile( p_luastate, filename.c_str()) || lua_pcall( p_luastate, 0, 0, 0)) {
        std::cout<<"script::script: lua file not loaded ("<<filename<<")"<<std::endl;
        p_luastate = NULL;
    }
}

void script::script::call( char *name, int id)
{
    int l_arguments = 0;

    if( p_luastate == NULL)
        return;

    lua_getglobal( p_luastate, name);
    // check for exist
    if( !lua_isfunction( p_luastate, -1)) {
        lua_pop( p_luastate,1);
        return;
    }

    // push arguments
    lua_pushnumber( p_luastate, id); l_arguments++;

    // call
    if (lua_pcall( p_luastate, l_arguments, 0, 0) != 0) {
        error( p_luastate, "script::start error running function `\"%s\"': %s", name, lua_tostring(p_luastate, -1));
    }
}

script::script::~script()
{
    if( p_luastate)
        lua_close( p_luastate);
}
