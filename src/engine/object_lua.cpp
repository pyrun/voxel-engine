#include <iostream>

#include "world.h"

world *p_target;

static int lua_print(lua_State* state) {
    if( !lua_isnumber( state, 1) ) {
        printf( "lua_print call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_print object with %d# not found\n", l_id);
        return 0;
    }

    // process the input
    int nargs = lua_gettop( state)-1;
    std::string l_text;
    for (int i=1; i <= nargs; ++i) {
		l_text += lua_tostring( state, i+1);
    }

    // call entity_list to add a new msg
    std::cout<<"lua_print("<<l_id<<"#):"<<l_text<<std::endl;

    // finish
    return 0;
}

void lua_object_install( lua_State *state) {
    // defined functions
    lua_pushcfunction( state, lua_print);
    lua_setglobal( state, "print");
}

void lua_object_set_targets( world *target) {
    p_target = target;
}
