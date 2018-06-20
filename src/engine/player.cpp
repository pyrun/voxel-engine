#include "player.h"

player::player( world *world)
{
    p_target_world = world;

    createObject();
}

player::~player()
{

}

void player::createObject() {
    // create player
    p_object_id = p_target_world->createObject( "player", p_target_world->getSpawnPoint() );
    //printf("position %d %d %d\n", (int)p_target_world->getSpawnPoint().x, (int)p_target_world->getSpawnPoint().y, (int)p_target_world->getSpawnPoint().z);
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
    glm::vec3 l_up(0.0f, 1.0f, 0.0f);
    object *l_player = p_target_world->getObject( p_object_id);
    float l_speed = 0.0009f;

    if( input->Map.Jump && l_player->getHit( physic::hit_side::ground) && l_player->getVerlocity().y < 0.0001f) {
        l_player->addVelocity( glm::vec3( 0.f, 0.052f, 0.f));
        l_player->setHit( physic::ground, false);
    }

    if( input->Map.Up && l_player->getHit( physic::hit_side::ground)) {
        glm::vec3 l_force = glm::cross( glm::normalize(glm::cross( camera->getUp(), camera->getForward())), l_up) * (float)delta * l_speed;

        l_player->addVelocity( l_force + glm::vec3( 0, 0.0, 0));
    }

    if( input->Map.Down && l_player->getHit( physic::hit_side::ground)) {
        glm::vec3 l_force = glm::cross( glm::normalize(glm::cross( camera->getUp(), camera->getForward())), l_up) * (float)delta * l_speed;

        l_player->addVelocity( -l_force + glm::vec3( 0, 0.0, 0));
    }

    if( input->Map.Right && l_player->getHit( physic::hit_side::ground) ) {
        glm::vec3 l_force = -glm::cross( camera->getUp(), camera->getForward()) * (float)delta * l_speed;
        l_player->addVelocity( l_force);
    }

    if( input->Map.Left && l_player->getHit( physic::hit_side::ground) ) {
        glm::vec3 l_force = glm::cross( camera->getUp(), camera->getForward()) * (float)delta * l_speed;
        l_player->addVelocity( l_force);
    }

    if( input->Map.Shift )
        ;
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
    glm::vec3 l_head = l_player->getType()->getHead();
    if( height == false)
        l_head.y = 0;
    return l_head+l_player->getPosition();
}

void player::setPosition( glm::vec3 position)
{
    p_position = position;
}
