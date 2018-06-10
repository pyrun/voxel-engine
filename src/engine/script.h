#ifndef SCRIPT_H
#define SCRIPT_H

// for compiler, he need to know how to link( c lib)
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>
#include <iostream>

class script
{
    public:
        script( const std::string& filename);
        ~script();

    protected:

    private:
        lua_State* p_luastate;
};

#endif // SCRIPT_H
