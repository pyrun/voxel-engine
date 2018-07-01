#include "player.h"

player::player( world *world)
{
    p_target_world = world;
    p_object_id = -1;

    createObject();
}

player::~player()
{

}

void player::createObject() {
    // create player
    if( p_target_world->getSpawnPoint() != glm::vec3( 0, 0, 0) )
        p_object_id = p_target_world->createObject( "player", p_target_world->getSpawnPoint() );
}

void player::raycastView( Input *input, glm::vec3 position, glm::vec3 lookat, int forward) {
    glm::vec3 l_postion_ray = position;
    glm::vec3 l_postion_prev = position;
    glm::vec3 l_block = { 0, 0, 0};
    glm::vec3 l_block_prev;
    bool l_found = false;

    if( p_target_world == NULL)
        return;

    for(int i = 0; i < forward; i++) {
        l_postion_prev = l_postion_ray;
        l_postion_ray += lookat * 0.01f;

        l_block.x = floorf( l_postion_ray.x);
        l_block.y = floorf( l_postion_ray.y);
        l_block.z = floorf( l_postion_ray.z);

        Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block);
        if( l_chunk) {
            glm::vec3 l_chunk_pos = l_chunk->getPos() * glm::ivec3( CHUNK_SIZE);
            if( l_chunk->getTile( l_block - l_chunk_pos) != EMPTY_BLOCK_ID) { // check for block
                l_found = true;
                break;
            }
        } else {
            p_target_world->addChunk( l_block / glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE), false );
        }
    }
    if( !l_found)
        return;

    // find out witch face we looking
    glm::vec3 l_postion_floor_prev;
    l_postion_floor_prev.x = floorf( l_postion_prev.x);
    l_postion_floor_prev.y = floorf( l_postion_prev.y);
    l_postion_floor_prev.z = floorf( l_postion_prev.z);

    l_block_prev = l_block;
    if( l_postion_floor_prev.x > l_block.x)
        l_block_prev.x++;
    else if( l_postion_floor_prev.x < l_block.x)
        l_block_prev.x--;
    else if( l_postion_floor_prev.y > l_block.y)
        l_block_prev.y++;
    else if( l_postion_floor_prev.y < l_block.y)
        l_block_prev.y--;
    else if( l_postion_floor_prev.z > l_block.z)
        l_block_prev.z++;
    else if( l_postion_floor_prev.z < l_block.z)
        l_block_prev.z--;

    // input handling
    if( input->Map.Place && !input->MapOld.Place) {
        Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block_prev);
        if( l_chunk) {
            p_target_world->changeBlock( l_chunk, l_block_prev, 8);
            printf( "%d %d %d\n", (int)l_block_prev.x, (int)l_block_prev.y, (int)l_block_prev.z);
        }
    }

    if( input->Map.Destory && !input->MapOld.Destory) {
        Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block);
        if( l_chunk)
            p_target_world->changeBlock( l_chunk, l_block, EMPTY_BLOCK_ID);
    }
}

void player::input( Input *input, Camera *camera, int delta) {
    object *l_player = p_target_world->getObject( p_object_id);
    if( !l_player ) {
        createObject();
        return;
    }

    if( !l_player->getScript())
        return;

    if( input->Map.Jump) {
        l_player->getScript()->call( "jump", l_player->getId(), delta);
    }

    if( input->Map.Up) {
        l_player->getScript()->call( "up", l_player->getId(), delta);
    }

    if( input->Map.Down) {
        l_player->getScript()->call( "down", l_player->getId(), delta);
    }

    if( input->Map.Right) {
        l_player->getScript()->call( "right", l_player->getId(), delta);
    }

    if( input->Map.Left) {
        l_player->getScript()->call( "left", l_player->getId(), delta);
    }
}

void player::changeWorldTo( world *world) {
    object *l_player = p_target_world->getObject( p_object_id);
    if( !l_player)
        return;
    // delete old object
    p_target_world->deleteObject( p_object_id);

    // set new world and create again
    p_target_world = world;
    createObject();
}

glm::vec3 player::getPositonHead( bool height) {
    object *l_player = p_target_world->getObject( p_object_id);
    if( l_player == NULL)
        return glm::vec3( 0, 0, 0);
    glm::vec3 l_head = l_player->getType()->getHead();
    if( height == false)
        l_head.y = 0;
    return l_head+l_player->getPosition();
}

void player::setPosition( glm::vec3 position)
{
    p_position = position;
}

void player::setName( std::string name) {
    p_name = name;
}

player_handle::player_handle()
{

}

player_handle::~player_handle()
{

}

bool player_handle::fileExists(std::string filename) {
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


void player_handle::load( std::string path) {
    DIR* l_handle;
    struct dirent* l_dirEntry;
    bool l_found_once = false;
    std::string l_name;
    std::string l_filepath;
    std::string l_path = path + "/";

    // open folder
    l_handle = opendir( l_path.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            l_name = l_dirEntry->d_name;
            l_filepath =  l_path + l_name;
            if( fileExists( l_filepath )) {
                //load_player( l_filepath);
                l_found_once = true;
            }
        }
    } else {
        printf( "player_handle::load can't open the \"%s\" folder\n", l_path.c_str());
    }
    closedir( l_handle );
}

void player_handle::load_player( std::string folder_player, world *world) {
    config *l_config = new config( folder_player + "/" + "player.xml");
    player *l_player  = new player( world);

    l_player->setName( l_config->get( "name", "player", "noname"));

    p_players.push_back( l_player);
    delete l_config;
    printf( "player_handle::load_player player \"%s\" loaded\n", l_player->getName().c_str());
}
