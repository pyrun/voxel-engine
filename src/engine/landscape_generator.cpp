#include "landscape_generator.h"

landscape_script::landscape_script( int id, lua_State *state)
{
    p_id = id;
    p_luastate = state;
}

landscape_script::~landscape_script()
{
    if( p_luastate) lua_close( p_luastate);
}

void landscape_script::setState( lua_State *state)
{
    p_luastate = state;
}

landscape::landscape( config *configuration)
{
    std::string l_script_folder;
    p_id = 0;

    // load scripts
    l_script_folder = configuration->get( "script_folder", "world", "world_generator/");
    loadScripts( l_script_folder);
}

landscape::~landscape()
{
    for( landscape_script *l_generator:p_generator)
        delete l_generator;
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

    // load file and and to vector
    luaL_loadfile( l_state, file.c_str());
    if( l_state)
        printf( "landscape::loadScript \"%s\" added with id #%d\n", file.c_str(), p_id);
    p_generator.push_back( new landscape_script( p_id, l_state) );
    p_id++;
}

/*float simplex_noise(int octaves, glm::vec3 pos){
    float value = 0.0;
    int i;
    for(i=0; i<octaves; i++){
        value += glm::perlin( glm::vec3(
            pos.x*pow(2, i),
            pos.y*pow(2, i),
            pos.z*pow(2, i))
        );
    }
    return value;
}

float distance( glm::vec3 pos1, glm::vec3 pos2)
{
    float dx = abs( pos2.x - pos1.x);
    float dz = abs( pos2.z - pos1.z);

    return sqrt( dx*dx + dz*dz );
}

void Landscape_Tree( Chunk* chunk, block_list* blocklist, glm::ivec3 position) {
    int l_leaf = blocklist->getByName( "leaf")->getID();
    int l_treewood = blocklist->getByName( "treewood")->getID();
    int l_glowcrystal = blocklist->getByName( "glowcrystal")->getID();

    chunk->set( position + glm::ivec3( 0, +1, 0), l_treewood, false);
    chunk->set( position + glm::ivec3( 0, +2, 0), l_treewood, false);
    chunk->set( position + glm::ivec3( 0, +3, 0), l_treewood, false);
    chunk->set( position + glm::ivec3( 0, +4, 0), l_treewood, false);

    chunk->set( position + glm::ivec3(-1, +4, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +4, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +4, 1), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +4,-1), l_leaf, false);

    chunk->set( position + glm::ivec3(-1, +4, 1), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +4, 1), l_leaf, false);
    chunk->set( position + glm::ivec3(-1, +4,-1), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +4,-1), l_leaf, false);

    chunk->set( position + glm::ivec3( 0, +5, 0), l_treewood, false);
    chunk->set( position + glm::ivec3(-1, +5, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +5, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +5, 1), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +5,-1), l_leaf, false);

    chunk->set( position + glm::ivec3( 0, +6, 0), l_leaf, false);
    chunk->set( position + glm::ivec3(-1, +5, 1), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +5, 1), l_leaf, false);
    chunk->set( position + glm::ivec3(-1, +5,-1), l_leaf, false);
    chunk->set( position + glm::ivec3( 1, +5,-1), l_leaf, false);

    chunk->set( position + glm::ivec3(-2, +4, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 2, +4, 0), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +4, 2), l_leaf, false);
    chunk->set( position + glm::ivec3( 0, +4,-2), l_leaf, false);
}

std::vector<glm::ivec3> Landscape_Generator( Chunk* chunk, block_list* blocklist) {
    srand( chunk->getSeed());
    std::vector<glm::ivec3> l_light_position;

    int l_stone = blocklist->getByName( "stone")->getID();
    int l_grass = blocklist->getByName( "grass")->getID();
    int l_glowcrystal = blocklist->getByName( "glowcrystal")->getID();
    int l_earth = blocklist->getByName( "earth")->getID();

    glm::ivec3 l_block;
    glm::vec3 l_real_pos;

    float l_size_inv = 30.f;

    glm::ivec3 l_position_chunk = chunk->getPos()*glm::ivec3(CHUNK_SIZE);
    glm::vec2 l_random = glm::vec2( rand(), rand() );

    for( l_block.x = 0; l_block.x < CHUNK_SIZE; l_block.x++) {

        for( l_block.z = 0; l_block.z < CHUNK_SIZE; l_block.z++) {

            float l_dstance_island = distance( glm::vec3(), l_position_chunk + l_block);

            int l_last_type;
            for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {

                l_real_pos = l_position_chunk + l_block;

                float l_noise_top = (- ( glm::perlin( (glm::vec2( l_real_pos.x, l_real_pos.z) + l_random) /l_size_inv ) ))*10.f;
                float l_noise_down = (glm::perlin( (glm::vec2( l_real_pos.x, l_real_pos.z) + l_random ) /l_size_inv ))*40.f;


                if(  l_dstance_island > 20.f) {
                    l_noise_top-= (l_dstance_island-20.f)/3.f;
                    l_noise_down+= (l_dstance_island-20.f)/3.f;
                }

                if( l_noise_top < l_real_pos.y)
                    continue;

                if( l_noise_down > l_real_pos.y)
                    continue;

                int l_type = l_stone;

                if( !(l_noise_top > l_real_pos.y+1) )
                    l_type = l_grass;
                else if( !(l_noise_top > l_real_pos.y+3) )
                    l_type = l_earth;

                if( rand()%40 == 1 && l_type == l_grass)
                    l_type = l_glowcrystal;
                if( rand()%33 == 1 && l_type == l_grass)
                    Landscape_Tree( chunk, blocklist, l_block);

                l_last_type = l_type;

                if( l_type == l_glowcrystal)
                    l_light_position.push_back( l_block);
                chunk->set( l_block, l_type, false);
            }
        }
    }
    return l_light_position;
}*/
