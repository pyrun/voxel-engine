#include <iostream>

#include "world.h"

world *p_target;

/// world
static int lua_getTileId(lua_State* state) {
    glm::vec3 l_tile_position;
    if( !lua_isnumber( state, 1) | !lua_isnumber( state, 2) | !lua_isnumber( state, 3)) {
        printf( "lua_getTileId call wrong argument\n");
        return 0;
    }

    // get position
    l_tile_position.x = lua_tonumber( state, 1);
    l_tile_position.y = lua_tonumber( state, 2);
    l_tile_position.z = lua_tonumber( state, 3);

    // get tile
    Chunk *l_chunk = p_target->getChunk( glm::ivec3( 0, 0, 0) );
    if( l_chunk == NULL) {
        lua_pushnumber( state, 0);
        return 1;
    }
    unsigned int l_tile = l_chunk->getTile( l_tile_position);

    // push the vector
    lua_pushnumber( state, l_tile);

    // finish
    return 1;
}

static int lua_setTileId(lua_State* state) {
    glm::vec3 l_tile_position;
    if( !lua_isnumber( state, 1) | !lua_isnumber( state, 2) | !lua_isnumber( state, 3) | !lua_isnumber( state, 4)) {
        printf( "lua_setTileId call wrong argument\n");
        return 0;
    }

    // id
    int l_tile = lua_tointeger( state, 1);

    // get position
    l_tile_position.x = lua_tonumber( state, 2);
    l_tile_position.y = lua_tonumber( state, 3);
    l_tile_position.z = lua_tonumber( state, 4);

    // set tile
    Chunk *l_chunk = p_target->getChunk( glm::ivec3( 0, 0, 0) );
    if( l_chunk == NULL) {
        lua_pushnumber( state, 0);
        return 1;
    }
    p_target->setTile( l_chunk, l_tile_position, l_tile);

    // push the vector
    lua_pushnumber( state, l_tile);

    // finish
    return 1;
}

/// Position
static int lua_getPosition(lua_State* state) {
    glm::vec3 l_position;
    if( !lua_isnumber( state, 1)) {
        printf( "lua_getPosition call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_getPosition object with %d# not found\n", l_id);
        return 0;
    }

    l_position = l_object->getPosition();

    // push the vector
    lua_pushnumber( state, l_position.x);
    lua_pushnumber( state, l_position.y);
    lua_pushnumber( state, l_position.z);

    // finish
    return 3;
}

static int lua_setPosition(lua_State* state) {
    glm::vec3 l_position;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_setPosition call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_position.x = lua_tonumber( state, 2);
    l_position.y = lua_tonumber( state, 3);
    l_position.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_setPosition object with %d# not found\n", l_id);
        return 0;
    }

    // set position
    l_object->setPosition( l_position);

    return 0;
}

static int lua_addPosition(lua_State* state) {
    glm::vec3 l_position;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_addPosition call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_position.x = lua_tonumber( state, 2);
    l_position.y = lua_tonumber( state, 3);
    l_position.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_addPosition object with %d# not found\n", l_id);
        return 0;
    }

    // set position
    l_object->addPosition( l_position);

    return 0;
}

/// Rotation
static int lua_getRotation(lua_State* state) {
    glm::vec3 l_rotation;
    if( !lua_isnumber( state, 1)) {
        printf( "lua_getPosition call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_getPosition object with %d# not found\n", l_id);
        return 0;
    }

    l_rotation = l_object->getRotation();

    // push the vector
    lua_pushnumber( state, l_rotation.x);
    lua_pushnumber( state, l_rotation.y);
    lua_pushnumber( state, l_rotation.z);

    // finish
    return 3;
}

static int lua_setRotation(lua_State* state) {
    glm::vec3 l_rotation;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_setRotation call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_rotation.x = lua_tonumber( state, 2);
    l_rotation.y = lua_tonumber( state, 3);
    l_rotation.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_setRotation object with %d# not found\n", l_id);
        return 0;
    }

    // set position
    l_object->setRotation( l_rotation);

    return 0;
}

static int lua_addRotation(lua_State* state) {
    glm::vec3 l_rotation;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_addRotation call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_rotation.x = lua_tonumber( state, 2);
    l_rotation.y = lua_tonumber( state, 3);
    l_rotation.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_addRotation object with %d# not found\n", l_id);
        return 0;
    }

    // set position
    l_object->addRotation( l_rotation);

    return 0;
}

/// velocity
static int lua_getVelocity(lua_State* state) {
    glm::vec3 l_velocity;
    if( !lua_isnumber( state, 1)) {
        printf( "lua_getVelocity call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_getVelocity object with %d# not found\n", l_id);
        return 0;
    }

    l_velocity = l_object->getVerlocity();

    // push the vector
    lua_pushnumber( state, l_velocity.x);
    lua_pushnumber( state, l_velocity.y);
    lua_pushnumber( state, l_velocity.z);

    // finish
    return 3;
}

static int lua_setVelocity(lua_State* state) {
    glm::vec3 l_velocity;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_setVelocity call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_velocity.x = lua_tonumber( state, 2);
    l_velocity.y = lua_tonumber( state, 3);
    l_velocity.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_setVelocity object with %d# not found\n", l_id);
        return 0;
    }

    // set position
    l_object->setVelocity( l_velocity);

    return 0;
}

static int lua_addVelocity(lua_State* state) {
    glm::vec3 l_velocity;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isnumber( state, 3) || !lua_isnumber( state, 4)) {
        printf( "lua_addVelocity call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);

    // vector lua
    l_velocity.x = lua_tonumber( state, 2);
    l_velocity.y = lua_tonumber( state, 3);
    l_velocity.z = lua_tonumber( state, 4);

    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_addVelocity object with %d# not found\n", l_id);
        return 0;
    }

    // add vector
    l_object->addVelocity( l_velocity);

    return 0;
}

/// hitbox
static int lua_getHitbox(lua_State* state) {
    glm::vec3 l_hitbox;
    if( !lua_isnumber( state, 1)) {
        printf( "lua_getHitbox call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_getHitbox object with %d# not found\n", l_id);
        return 0;
    }

    l_hitbox = l_object->getType()->getHitbox();

    // push the vector
    lua_pushnumber( state, l_hitbox.x);
    lua_pushnumber( state, l_hitbox.y);
    lua_pushnumber( state, l_hitbox.z);

    // finish
    return 3;
}

/// hit handling
static int lua_getHit(lua_State* state) {
    bool l_hit;
    physic::hit_side l_side;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2)) {
        printf( "lua_getHit call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    int l_side_number = lua_tointeger( state, 2);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_getHit object with %d# not found\n", l_id);
        return 0;
    }

    switch( l_side_number) {
        case 0: l_side = physic::hit_side::ground; break;
        case 1: l_side = physic::hit_side::top; break;
        case 2: l_side = physic::hit_side::west; break;
        case 3: l_side = physic::hit_side::east; break;
        case 4: l_side = physic::hit_side::north; break;
        case 5: l_side = physic::hit_side::south; break;
    }

    l_hit = l_object->getHit( l_side);

    // push the vector
    lua_pushboolean( state, l_hit);

    // finish
    return 1;
}

static int lua_setHit(lua_State* state) {
    bool l_hit;
    physic::hit_side l_side;
    if( !lua_isnumber( state, 1) || !lua_isnumber( state, 2) || !lua_isboolean( state, 3)) {
        printf( "lua_setHit call wrong argument\n");
        return 0;
    }

    // get obj
    int l_id = lua_tointeger( state, 1);
    int l_side_number = lua_tointeger( state, 2);
    bool l_value = lua_toboolean( state, 3);
    object *l_object = p_target->getObject( l_id);
    if( l_object == NULL) {
        printf( "lua_setHit object with %d# not found\n", l_id);
        return 0;
    }

    switch( l_side_number) {
        case 0: l_side = physic::hit_side::ground; break;
        case 1: l_side = physic::hit_side::top; break;
        case 2: l_side = physic::hit_side::west; break;
        case 3: l_side = physic::hit_side::east; break;
        case 4: l_side = physic::hit_side::north; break;
        case 5: l_side = physic::hit_side::south; break;
    }

    l_object->setHit( l_side, l_value);
    return 0;
}


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
    lua_pushcfunction( state, lua_getTileId);
    lua_setglobal( state, "getTileId");
    lua_pushcfunction( state, lua_setTileId);
    lua_setglobal( state, "setTileId");

    lua_pushcfunction( state, lua_getPosition);
    lua_setglobal( state, "getPosition");
    lua_pushcfunction( state, lua_setPosition);
    lua_setglobal( state, "setPosition");
    lua_pushcfunction( state, lua_addPosition);
    lua_setglobal( state, "addPosition");

    lua_pushcfunction( state, lua_getRotation);
    lua_setglobal( state, "getRotation");
    lua_pushcfunction( state, lua_setRotation);
    lua_setglobal( state, "setRotation");
    lua_pushcfunction( state, lua_addRotation);
    lua_setglobal( state, "addRotation");

    lua_pushcfunction( state, lua_getVelocity);
    lua_setglobal( state, "getVelocity");
    lua_pushcfunction( state, lua_setVelocity);
    lua_setglobal( state, "setVelocity");
    lua_pushcfunction( state, lua_addVelocity);
    lua_setglobal( state, "addVelocity");

    lua_pushcfunction( state, lua_getHitbox);
    lua_setglobal( state, "getHitbox");

    lua_pushcfunction( state, lua_getHit);
    lua_setglobal( state, "getHit");
    lua_pushcfunction( state, lua_setHit);
    lua_setglobal( state, "setHit");

    lua_pushcfunction( state, lua_print);
    lua_setglobal( state, "print");
}

void lua_object_set_targets( world *target) {
    p_target = target;
}
