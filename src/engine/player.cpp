#include "player.h"

player::player( world *world)
{
    p_target_world = world;
    p_object_id = -1;

    //createObject();
}

player::~player()
{

}

void player::createObject() {
    // create player
    if( p_target_world && p_target_world->getSpawnPoint() != glm::vec3( 0, 0, 0) )
        p_object_id = p_target_world->createObject( "player", p_target_world->getSpawnPoint() );
}

void player::raycastView( input::handle *input, glm::vec3 position, glm::vec3 lookat, int forward) {
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
    if( input->mappping.place && !input->mappping_previously.place) {
        Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block_prev);
        if( l_chunk) {
            p_target_world->changeBlock( l_chunk, l_block_prev, 1);
            printf( "player::raycastView %d %d %d\n", (int)l_block_prev.x, (int)l_block_prev.y, (int)l_block_prev.z);
        }
    }

    if( input->mappping.destory && !input->mappping_previously.destory) {
        Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block);
        if( l_chunk)
            p_target_world->changeBlock( l_chunk, l_block, EMPTY_BLOCK_ID);
    }
}

player_teleport *player::handleTeleport( input::handle *input, glm::vec3 position, glm::vec3 lookat, int forward) {
    object *l_object = getObject();
    if( !l_object)
        return NULL;
    glm::vec3 l_postion_ray = l_object->getType()->getHead() + l_object->getPosition();
    glm::vec3 l_postion_prev = l_postion_ray;
    glm::vec3 l_rotation = l_object->getRotation();
    glm::vec3 l_block_size = glm::vec3( 0.05);
    glm::vec3 l_velocity = glm::vec3( 0, 0.1, 0);
    glm::vec3 l_color = glm::vec3( 0, 1, 0);
    glm::vec3 l_block = { 0, 0, 0};

    p_debug_draw.clear();

    //p_debug_draw.cube( glm::vec3( 1), glm::vec3( 1), glm::vec3( 1, 0, 0));

    bool l_found = false;
    for(int i = 0; i < forward; i++) {
        l_postion_prev = l_postion_ray;
        l_postion_ray += lookat * 0.25f;



        l_block.x = floorf( l_postion_ray.x );
        l_block.y = floorf( l_postion_ray.y );
        l_block.z = floorf( l_postion_ray.z );

        if( 1) {
            Chunk *l_chunk = p_target_world->getChunkWithPosition( l_block);
            if( l_chunk) {
                glm::vec3 l_chunk_pos = l_chunk->getPos() * glm::ivec3( CHUNK_SIZE);
                if( l_chunk->getTile( l_block - l_chunk_pos) != EMPTY_BLOCK_ID) { // check for block
                    l_found = true;
                    //l_color = glm::vec3( 1, 0, 0);
                    //printf( "%f %f %f\n", l_block.x, l_block.y, l_block.z);
                } else {
                    //l_color = glm::vec3( 0, 1, 0);
                }
            }
        }

        if( !l_found) {
            p_debug_draw.cube( l_postion_ray-(l_block_size/glm::vec3(2)), l_block_size, l_color);
        } else {
            l_block.x = floorf( l_postion_ray.x);
            l_block.y = floorf( l_postion_ray.y);
            l_block.z = floorf( l_postion_ray.z);

            p_debug_draw.cube( l_block-(glm::vec3( 0.1)), glm::vec3( 1.2), glm::vec3( 0, 0, 1));
            p_debug_draw.setChange();

            player_teleport *l_port = new player_teleport;
            l_port->position = l_block;
            return l_port;
        }

        l_velocity += glm::vec3( 0, -0.01, 0);
        l_postion_ray += l_velocity;
    }

    p_debug_draw.setChange();

    return NULL;
}

void player::input( input::handle *input, Camera *camera, int delta) {
    object *l_player = p_target_world->getObject( p_object_id);
    if( !l_player ) {
        return;
    }

    if( !l_player->getScript())
        return;

    if( input->mappping.jump) {
        l_player->getScript()->call( "jump", l_player->getId(), delta);
    }

    if( input->mappping.up) {
        l_player->getScript()->call( "up", l_player->getId(), delta);
    }

    if( input->mappping.down) {
        l_player->getScript()->call( "down", l_player->getId(), delta);
    }

    if( input->mappping.right) {
        l_player->getScript()->call( "right", l_player->getId(), delta);
    }

    if( input->mappping.left) {
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
    if( getWorld() == NULL)
        return glm::vec3( 0, 0, 0);
    object *l_player = p_target_world->getObject( p_object_id);
    if( l_player == NULL)
        return glm::vec3( 0, 0, 0);
    glm::vec3 l_head = l_player->getType()->getHead();
    if( height == false)
        l_head.y = 0;
    return l_head+l_player->getPosition();
}

object *player::getObject() {
    return getWorld()->getObject( p_object_id);
}

void player::setPosition( glm::vec3 position)
{
    p_position = position;
}

void player::setName( std::string name) {
    p_name = name;
}

void player::setId( unsigned int id) {
    p_object_id = id;
}

void player::setGUID( RakNet::RakNetGUID guid) {
    p_guid = guid;
}

void player::setWorld( world *world) {
    p_target_world = world;
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

player *player_handle::createPlayer( world *world) {
    player *l_player  = new player( world);

    // set values
    l_player->setName( "noname");

    // return pointer
    p_players.push_back( l_player);
    return l_player;
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
                //load_player( l_filepath, );
                l_found_once = true;
            }
        }
    } else {
        printf( "player_handle::load can't open the \"%s\" folder\n", l_path.c_str());
    }
    closedir( l_handle );
}

void player_handle::load_player( std::string folder_player, world *world) {
    std::string l_file = folder_player + "/" + "player.xml";
    if( fileExists( l_file) == false)
        mkdir( folder_player.c_str());
    config *l_config = new config( l_file);
    player *l_player  = new player( world);

    l_player->setName( l_config->get( "name", "player", "noname"));

    p_players.push_back( l_player);
    delete l_config;
    printf( "player_handle::load_player player \"%s\" loaded\n", l_player->getName().c_str());
}

void player_handle::deletePlayerByGUID( RakNet::RakNetGUID guid) {
    for( player *l_player:p_players){
        if( l_player->getGUID() == guid)
            delete l_player;
    }
}

player *player_handle::getPlayerByName( std::string name) {
    for( player *l_player:p_players){
        if( l_player->getName() == name)
            return l_player;
    }
    return NULL;
}

player *player_handle::getPlayerByGUID( RakNet::RakNetGUID guid) {
    for( player *l_player:p_players){
        if( l_player->getGUID() == guid)
            return l_player;
    }
    return NULL;
}

player *player_handle::getPlayerByObject( object *object) {
    for( player *l_player:p_players){
        if( l_player->getId() == object->getId())
            return l_player;
    }
    return NULL;
}
