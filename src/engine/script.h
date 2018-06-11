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
#include <vector>

namespace script {

    class libs {
        public:
            libs();
            ~libs();

            void (*func)( lua_State *);
            std::string name;
    };

    void add_lib( std::string name, void (*func)( lua_State *));
    void install_libs( lua_State *state);

    class script
    {
        public:
            script( const std::string& filename);
            ~script();

            void start();
            void timer();
            void end();

            void addFunction( );
        protected:

        private:
            lua_State* p_luastate;
    };

};

#endif // SCRIPT_H
