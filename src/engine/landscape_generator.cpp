#include "landscape_generator.h"

block_list* public_blocklist;

int public_number = 0;
Chunk *public_chunk_list[ MAX_CHUNKS_GENERATE];

static int lua_getByName (lua_State *state) {
    std::string l_name= lua_tostring( state, 1);

    block *l_block = public_blocklist->getByName( l_name);
    if( !l_block) {
        printf( "lua_getByName don't found the type \"%s\"\n", l_name.c_str());
        return 0;
    }
    int l_type = l_block->getID();

    lua_pushnumber( state, l_type);
    return 1;
}

static int lua_distanceVec2 (lua_State *state) {
    glm::vec2 l_pos1;
    glm::vec2 l_pos2;

    l_pos1.x = lua_tonumber( state, 1);
    l_pos1.y = lua_tonumber( state, 2);
    l_pos2.x = lua_tonumber( state, 3);
    l_pos2.y = lua_tonumber( state, 4);

    float dx = abs( l_pos2.x - l_pos1.x);
    float dz = abs( l_pos2.y - l_pos1.y);

    lua_pushnumber( state, sqrt( dx*dx + dz*dz ));
    return 1;
}

static int lua_perlinVec2 (lua_State *state) {
    glm::vec2 l_position;

    l_position.x = lua_tonumber( state, 1);
    l_position.y = lua_tonumber( state, 2);

    lua_pushnumber( state, (float)glm::perlin( l_position) );
    return 1;
}

static int lua_rand (lua_State *state) {
    int l_max = lua_tonumber( state, 1);

    lua_pushnumber( state, rand()%l_max );
    return 1;
}

static int lua_setBlock (lua_State *state) {
    glm::ivec3 l_position;
    int l_block = lua_tonumber( state, 2);
    int l_chunk_id = lua_tonumber( state, 1);

    l_position.x = lua_tonumber( state, 3);
    l_position.y = lua_tonumber( state, 4);
    l_position.z = lua_tonumber( state, 5);

    if( public_chunk_list[ l_chunk_id])
        public_chunk_list[ l_chunk_id]->set( l_position, l_block, false);
    else
        printf( "lua_setBlock chunk didnt found!\n");
    return 0;
}

landscape_script::landscape_script( int id, lua_State *state)
{
    p_id = id;
    p_luastate = state;

    // add lua function
    lua_pushcfunction( state, lua_getByName);
    lua_setglobal( state, "getByName");

    lua_pushcfunction( state, lua_distanceVec2);
    lua_setglobal( state, "distanceVec2");

    lua_pushcfunction( state, lua_perlinVec2);
    lua_setglobal( state, "perlinVec2");

    lua_pushcfunction( state, lua_rand);
    lua_setglobal( state, "rand");

    lua_pushcfunction( state, lua_setBlock);
    lua_setglobal( state, "setBlock");
}

landscape_script::~landscape_script()
{
    if( p_luastate)
        lua_close( p_luastate);
}

void landscape_script::setState( lua_State *state)
{
    p_luastate = state;
}

std::vector<glm::ivec3> landscape_script::generator( Chunk* chunk, block_list* blocklist)
{
    int l_type = 0;
    bool l_light = false;
    srand( chunk->getSeed());
    std::vector<glm::ivec3> l_lights;
    public_blocklist = blocklist;

    // set chunk id
    int l_id_chunk = ++public_number;
    public_chunk_list[ l_id_chunk] = chunk;

    glm::ivec3 l_position_chunk = chunk->getPos()*glm::ivec3(CHUNK_SIZE);
    glm::ivec3 l_random = glm::ivec3( rand(), rand(), rand());

    glm::ivec3 l_block;
    glm::vec3 l_real_pos;

    if( p_luastate) {
        lua_pcall( p_luastate, 0, 0, 0);

        for( l_block.x = 0; l_block.x < CHUNK_SIZE; l_block.x++) {

            l_real_pos = l_position_chunk + l_block;

            lua_getglobal( p_luastate, "rowX");
            if( lua_isfunction( p_luastate, -1)) {
                // block position
                lua_pushnumber( p_luastate, l_real_pos.x );
                lua_pushnumber( p_luastate, l_real_pos.y );
                lua_pushnumber( p_luastate, l_real_pos.z );
                // random
                lua_pushnumber( p_luastate, l_random.x );
                lua_pushnumber( p_luastate, l_random.y );
                lua_pushnumber( p_luastate, l_random.z );
                lua_pcall( p_luastate, 6, 0, 0);
            }

            for( l_block.z = 0; l_block.z < CHUNK_SIZE; l_block.z++) {

                l_real_pos = l_position_chunk + l_block;

                lua_getglobal( p_luastate, "rowZ");
                if( lua_isfunction( p_luastate, -1)) {
                    // block position
                    lua_pushnumber( p_luastate, l_real_pos.x );
                    lua_pushnumber( p_luastate, l_real_pos.y );
                    lua_pushnumber( p_luastate, l_real_pos.z );
                    // random
                    lua_pushnumber( p_luastate, l_random.x );
                    lua_pushnumber( p_luastate, l_random.y );
                    lua_pushnumber( p_luastate, l_random.z );
                    lua_pcall( p_luastate, 6, 0, 0);
                }

                for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {

                    l_real_pos = l_position_chunk + l_block;

                    lua_getglobal( p_luastate, "block");

                    // chunk id
                    lua_pushnumber( p_luastate, l_id_chunk );

                    // chunk position
                    lua_pushnumber( p_luastate, l_position_chunk.x );
                    lua_pushnumber( p_luastate, l_position_chunk.y );
                    lua_pushnumber( p_luastate, l_position_chunk.z );

                    // block position
                    lua_pushnumber( p_luastate, l_real_pos.x );
                    lua_pushnumber( p_luastate, l_real_pos.y );
                    lua_pushnumber( p_luastate, l_real_pos.z );

                    // random
                    lua_pushnumber( p_luastate, l_random.x );
                    lua_pushnumber( p_luastate, l_random.y );
                    lua_pushnumber( p_luastate, l_random.z );

                    lua_pcall( p_luastate, 10, 2, 0);
                    l_type = lua_tonumber( p_luastate, -1);
                    lua_pop( p_luastate, 1);
                    l_light = lua_toboolean( p_luastate, -1);
                    lua_pop( p_luastate, 1);

                    if( l_type == EMPTY_BLOCK_ID)
                        continue;

                    if( l_light)
                        l_lights.push_back( l_block);

                    chunk->set( l_block, l_type, false);
                }
            }
        }
    }

    public_chunk_list[ l_id_chunk] = NULL;

    return l_lights;
}

landscape::landscape( config *configuration)
{
    std::string l_script_folder;
    p_id = 0;

    // load scripts
    l_script_folder = configuration->get( "script_folder", "world", "world_generator/");
    loadScripts( l_script_folder);

    // clear the list
    for( int i = 0; i < MAX_CHUNKS_GENERATE; i++)
        public_chunk_list[ i] = NULL;
}

landscape::~landscape()
{
    for( landscape_script *l_generator:p_generator)
        delete l_generator;
}

landscape_script *landscape::getGenerator( Chunk *chunk)
{
    int l_seed = chunk->getSeed();
    return p_generator[ l_seed%p_generator.size()];
}

bool landscape::fileExists(std::string filename) {
    std::ifstream l_file;
    // file open and close
    l_file.open ( filename.c_str());
    if (l_file.is_open()) {
        l_file.close();
        return true;
    }
    l_file.close();
    return false;
}

bool landscape::loadScripts( std::string path)
{
    DIR* l_handle;
    struct dirent* l_dirEntry;
    std::string l_name;
    std::string l_filepath;

    // open folder
    l_handle = opendir( path.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            l_name = l_dirEntry->d_name;
            l_filepath =  path + l_name;
            if( fileExists( l_filepath ))
                loadScript( l_filepath);
        }
    } else {
        printf( "landscape::loadScripts can't open the \"%s\" folder\n", path.c_str());
    }
    closedir( l_handle );
}

bool landscape::loadScript( std::string file)
{
    lua_State* l_state = luaL_newstate();
    luaL_openlibs( l_state);

    // load file and and to vector
    luaL_loadfile( l_state, file.c_str());
    if( l_state)
        printf( "landscape::loadScript \"%s\" added with id #%d\n", file.c_str(), p_id);
    p_generator.push_back( new landscape_script( p_id, l_state) );
    p_id++;
}
