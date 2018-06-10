#include "script.h"

script::script( const std::string& filename)
{
    p_luastate = luaL_newstate();
    if (luaL_loadfile( p_luastate, filename.c_str()) || lua_pcall( p_luastate, 0, 0, 0)) {
        std::cout<<"script::script: lua file not loaded ("<<filename<<")"<<std::endl;
        p_luastate = NULL;
    }
}

script::~script()
{
    if( p_luastate)
        lua_close( p_luastate);
}
