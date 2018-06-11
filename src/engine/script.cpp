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

script::script::script( const std::string& filename)
{
    p_luastate = luaL_newstate();
    if (luaL_loadfile( p_luastate, filename.c_str()) || lua_pcall( p_luastate, 0, 0, 0)) {
        std::cout<<"script::script: lua file not loaded ("<<filename<<")"<<std::endl;
        p_luastate = NULL;
    }
}

script::script::~script()
{
    if( p_luastate)
        lua_close( p_luastate);
}
