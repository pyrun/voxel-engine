#include "world.h"
#include <time.h>
#include "../system/timer.h"

static int world_thread_handle(void *data)
{
    for ( ;; ) {
        world *l_world = (world*)data;

        l_world->process_thrend_handle();

        if( l_world->getDestory() )
            break;

        SDL_Delay(1);
    }

    return 0;
}

static int world_thread_update(void *data)
{
    for ( ;; ) {
        world *l_world = (world*)data;

        l_world->process_thrend_update();

        if( l_world->getDestory() )
            break;

        SDL_Delay(1);
    }

    return 0;
}

static int world_thread_physic(void *data)
{
    for ( ;; ) {
        world *l_world = (world*)data;

        l_world->process_thrend_physic();

        if( l_world->getDestory() )
            break;

        SDL_Delay(1);
    }

    return 0;
}

world::world( block_list* block_list, std::string name, object_handle *objectHandle) {
    p_buysvector = false;
    p_chunk_amount = 0;
    p_chunk_visible_amount = 0;
    p_chunk_start = NULL;
    p_chunk_last = NULL;
    p_world_tree_empty = true;
    p_blocklist = block_list;
    p_destroy = false;
    p_time = SDL_GetTicks();
    p_name = name;
    p_pointer_object_handle = objectHandle;
    p_object_id = 0;
    p_gravity = glm::vec3( 0, -100.0/1000., 0);

    // creating two mutex
    p_mutex_handle = SDL_CreateMutex();
    p_mutex_physic = SDL_CreateMutex();

    // creating the threads
    for( int i = 0; i < WORLD_HANDLE_THRENDS; i++)
        p_thread_handle[i] = SDL_CreateThread( world_thread_handle, "world_thread_handle", (void *)this);
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        p_thread_update[i] = SDL_CreateThread( world_thread_update, "world_thread_update", (void *)this);
    p_thread_physic = SDL_CreateThread( world_thread_physic, "world_thread_physic", (void *)this);
}

world::~world() {
    int l_return; // didnt use

    // save all
    save();

    // wait for thread end
    p_destroy = true;
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        SDL_WaitThread( p_thread_update[i], &l_return);
    for( int i = 0; i < WORLD_HANDLE_THRENDS; i++)
        SDL_WaitThread( p_thread_handle[i], &l_return);
    SDL_WaitThread( p_thread_physic, &l_return);

    // destroy the mutex
    SDL_DestroyMutex( p_mutex_handle);
    SDL_DestroyMutex( p_mutex_physic);

    // free objects
    for( object* l_object:p_objects)
        delete l_object;

    // free ram
    deleteChunks( p_chunk_start);
    while( p_chunk_amount != 0) {
        SDL_Delay(1);
    }
}

int world::createObject( std::string name, glm::vec3 position) {
    object *l_object;

    object_type *l_type = p_pointer_object_handle->get( name);

    if( !l_type) {
        printf( "world::createObject type \"%s\"not found\n", name.c_str());
        return -1;
    }
    l_object = new object();
    l_object->setPosition( position);
    l_object->setType( l_type);
    l_object->setId( ++p_object_id);

    // creating the physic body
    /*b3BodyDef l_bdef;
    // set up def.
    l_bdef.type = b3BodyType::e_dynamicBody;
    l_bdef.fixedRotationX = true;
    l_bdef.fixedRotationY = false;
    l_bdef.fixedRotationZ = true;
    b3Body* l_body = getPhysicWorld()->CreateBody(l_bdef);
    l_object->setBody( l_body);*/

    p_objects.push_back( l_object);

    return 1;
}

object *world::getObject( int id) {
    for( object *l_object:p_objects)
        if( l_object->getId() == id)
            return l_object;
    return NULL;
}

int world::getTile( glm::ivec3 position) {
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL)
            break;
        glm::ivec3 p_chunk_position = position - node->getPos()*glm::ivec3( CHUNK_SIZE);

        // X - Achse
        if( p_chunk_position.x < 0 ) {
            node = node->next;
            continue;
        }
        if( p_chunk_position.x >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if( p_chunk_position.y < 0 ) {
            node = node->next;
            continue;
        }
        if( p_chunk_position.y >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if( p_chunk_position.z < 0 ) {
            node = node->next;
            continue;
        }
        if( p_chunk_position.z >= CHUNK_SIZE) {
            node = node->next;
            continue;
        }

        int l_tile = node->getTile( p_chunk_position );
        if( l_tile == EMPTY_BLOCK_ID)
            break;
        return l_tile;
    }
    return NULL;
}

Chunk* world::getChunkWithPosition( glm::ivec3 position) {
    glm::ivec3 l_chunk;
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL)
            break;
        l_chunk = position - node->getPos() * glm::ivec3( CHUNK_SIZE);

        // X - Achse
        if( l_chunk.x < 0 ) {
            node = node->next;
            continue;
        }
        if( l_chunk.x >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if( l_chunk.y < 0 ) {
            node = node->next;
            continue;
        }
        if( l_chunk.y >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if( l_chunk.z < 0 ) {
            node = node->next;
            continue;
        }
        if( l_chunk.z >= CHUNK_SIZE) {
            node = node->next;
            continue;
        }
        return node;
    }
    return NULL;
}

void world::changeBlock( Chunk *chunk, glm::vec3 position, int id) {
    world_change_block l_change;
    l_change.position = position;
    l_change.id = id;
    l_change.chunk = chunk;

    p_change_blocks.push( l_change);
    //p_change_blocks.push_back( l_change);
}

void world::setTile( Chunk *chunk, glm::ivec3 position, int id) {
    block *l_type;
    glm::ivec3 l_chunk_position;
    glm::ivec3 l_position;

    if( chunk == NULL) {
        printf( "world::setTile: chunk is defined as NULL\n");
        return;
    }
    // calculate position
    l_chunk_position = chunk->getPos() * glm::ivec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    l_position = position - l_chunk_position;

    l_type = p_blocklist->get( id);

    chunk->set( l_position, id, false);

    if( l_type == NULL || l_type->isLight() == false )
        delTorchlight( chunk, position);
    else
        addTorchlight( chunk, position, l_type->getLighting());
}

void world::addTorchlight( Chunk *chunk, glm::ivec3 position, int value) {
    // calculate the position by chunk
    glm::ivec3 l_chunk_position = chunk->getPos() * glm::ivec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    glm::ivec3 l_position = position - l_chunk_position;

    //printf( "world::addTorchlight %d/%d/%d\n", position.x, position.y, position.z);

    // add too queue
    chunk->setTorchlight( l_position, value);
    p_lightsAdd.emplace( l_position, chunk);
}

void world::delTorchlight( Chunk *chunk, glm::ivec3 position) {
    int l_light;

    // calculate the position by chunk
    glm::ivec3 l_chunk_position = chunk->getPos() * glm::ivec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    position = position - l_chunk_position;

    //printf( "world::delTorchlight %d/%d/%d\n", position.x, position.y, position.z);

    // add too queue
    l_light = chunk->getTorchlight( position);
    chunk->setTorchlight( position, 0);
    p_lightsDel.emplace( position, chunk, l_light);
}

void world::process_thrend_handle() {
    std::vector<glm::ivec3> l_lights;

    SDL_LockMutex ( p_mutex_handle);

    // creating chunks
    while( p_creatingList.empty() == false) {
        // Get a reference to the front node.
        world_data_list &l_list_node = p_creatingList.front();

        glm::ivec3 l_position = l_list_node.position;
        Chunk *l_node = getChunk( l_position);
        bool l_landscape = l_list_node.landscape;

        p_creatingList.pop();

        if( l_node == NULL) {
            Chunk *l_node = createChunk( l_position);
            p_landscape.push( world_data_list(l_position, l_landscape) );
        }
    }
    SDL_UnlockMutex ( p_mutex_handle);

    // landscape generator
    while( p_landscape.empty() == false) {
        //SDL_LockMutex ( p_mutex_handle);
        // Get a reference to the front node.
        world_data_list l_list_node = p_landscape.front();

        glm::ivec3 l_position = l_list_node.position;
        Chunk *l_node = getChunk( l_position);
        bool l_landscape = l_list_node.landscape;

        p_landscape.pop();
        //SDL_UnlockMutex ( p_mutex_handle);

        if( l_node != NULL) {

            if( l_landscape) {
                //SDL_LockMutex ( p_mutex_handle);
                l_lights = p_landscape_generator->getGenerator( l_node)->generator( l_node, p_blocklist);
                l_node->changed( true);
                //SDL_UnlockMutex ( p_mutex_handle);
            }

            for( glm::ivec3 l_light:l_lights) {
                int l_block_id = l_node->getTile( l_light);
                block *l_block = p_blocklist->get( l_block_id);
                if( l_block)
                    addTorchlight( l_node, l_light + l_node->getPos() * glm::ivec3( CHUNK_SIZE), l_block->getLighting());
            }
        }
        break;
    }

    // deleting chunks
    while( p_deletingList.empty() == false) {
        // Get a reference to the front node.
        world_data_list &l_list_node = p_deletingList.front();

        glm::ivec3 l_position = l_list_node.position;
        Chunk *l_node = getChunk( l_position);
        bool l_landscape = l_list_node.landscape;

        p_deletingList.pop();

        if( l_node != NULL) {
            SDL_LockMutex ( p_mutex_handle);
            SDL_LockMutex ( p_mutex_physic);
            deleteChunk( l_node);
            SDL_UnlockMutex ( p_mutex_handle);
            SDL_UnlockMutex ( p_mutex_physic);
        }
    }

    glm::ivec3 l_shift_matrix[6] = { { -1,  0,  0},
                                     { +1,  0,  0},
                                     {  0, -1,  0},
                                     {  0, +1,  0},
                                     {  0,  0, -1},
                                     {  0,  0, +1}};
    std::vector<glm::ivec3> l_chunk_update_list;

    // ligting
    if( p_landscape.empty() == false)
        return;

    int l_status_mutex = SDL_TryLockMutex( p_mutex_handle);

    if( l_status_mutex != 0) // mutex lock for this tread
        return;

    while( p_lightsDel.empty() == false) {
        // Get a reference to the front node.
        world_light_node &l_light_node = p_lightsDel.front();

        glm::ivec3 l_position = l_light_node.position;
        Chunk* l_chunk = l_light_node.chunk;
        int l_light_level = (int)l_light_node.strength;

        // Pop the front node off the queue. We no longer need the node reference
        p_lightsDel.pop();

        if( l_chunk == NULL)
            continue;

        for( int i = 0; i < 6; i++) {
            int l_neighborLevel = l_chunk->getTorchlight( l_position + l_shift_matrix[i] );
            if (l_neighborLevel != 0 && l_neighborLevel < l_light_level) {
                // Set its light level
                Chunk *l_light_chunk = l_chunk->setTorchlight( l_position + l_shift_matrix[i] , 0);

                // check and add to the queue
                bool l_found = false;
                for( glm::ivec3 l_chunk_position:l_chunk_update_list)
                    if( l_chunk_position == l_light_chunk->getPos())
                        l_found = true;
                if( !l_found)
                    l_chunk_update_list.push_back( l_light_chunk->getPos());

                // Emplace new node to queue. (could use push as well)
                p_lightsDel.emplace( l_position + l_shift_matrix[i] , l_chunk, l_neighborLevel);

            } else if (l_neighborLevel >= l_light_level) {
                p_lightsAdd.emplace( l_position + l_shift_matrix[i] , l_chunk);
            }

            if( l_position.x + l_shift_matrix[i].x < 0 || l_position.y + l_shift_matrix[i].y < 0 || l_position.z + l_shift_matrix[i].z < 0 ||
                l_position.x + l_shift_matrix[i].x > CHUNK_SIZE-1 || l_position.y + l_shift_matrix[i].y > CHUNK_SIZE-1 || l_position.z + l_shift_matrix[i].z > CHUNK_SIZE-1 ) {

                Chunk *l_light_chunk = getChunkWithPosition( l_chunk->getPos()*CHUNK_SIZE + l_position + l_shift_matrix[i]);

                if( l_light_chunk) {
                    // check and add to the queue
                    bool l_found = false;
                    for( glm::ivec3 l_chunk_position:l_chunk_update_list)
                        if( l_chunk_position == l_light_chunk->getPos())
                            l_found = true;
                    if( !l_found)
                        l_chunk_update_list.push_back( l_light_chunk->getPos());
                }
            }
        }
    }

    while( p_lightsAdd.empty() == false) {
        // Get a reference to the front node.
        world_light_node &l_light_node = p_lightsAdd.front();

        glm::ivec3 l_position = l_light_node.position;
        Chunk* l_chunk = l_light_node.chunk;

        if( l_chunk == NULL) {
            p_lightsAdd.pop();
            continue;
        }

        // Grab the light level of the current node
        int l_lightLevel = l_chunk->getTorchlight( l_position);

        // Pop the front node off the queue. We no longer need the node reference
        p_lightsAdd.pop();

        for( int i = 0; i < 6; i++) {
            if ( l_chunk->checkTile( l_position + l_shift_matrix[i] ) == false &&
                l_chunk->getTorchlight( l_position + l_shift_matrix[i]) + 2 <= l_lightLevel) {
                // Set its light level
                Chunk *l_light_chunk = l_chunk->setTorchlight( l_position + l_shift_matrix[i], l_lightLevel - 1);

                // check and add to the queue
                bool l_found = false;
                for( glm::ivec3 l_chunk_position:l_chunk_update_list)
                    if( l_chunk_position == l_light_chunk->getPos())
                        l_found = true;
                if( !l_found)
                    l_chunk_update_list.push_back( l_light_chunk->getPos());

                // Construct index
                glm::ivec3 l_new_position = l_position + l_shift_matrix[i];
                // Emplace new node to queue
                p_lightsAdd.emplace( l_new_position, l_chunk);
            }

            Chunk *l_light_chunk = getChunkWithPosition( l_chunk->getPos()*CHUNK_SIZE + l_position + l_shift_matrix[i]);

            if( l_light_chunk) {
                // check and add to the queue
                bool l_found = false;
                for( glm::ivec3 l_chunk_position:l_chunk_update_list)
                    if( l_chunk_position == l_light_chunk->getPos())
                        l_found = true;
                if( !l_found)
                    l_chunk_update_list.push_back( l_light_chunk->getPos());
            }
        }
    }

    for( glm::ivec3 l_position_chunk: l_chunk_update_list) {
        p_update_changes.push( world_data_list( l_position_chunk, false) );
    }

    SDL_UnlockMutex ( p_mutex_handle); // lingting ending
}

void world::process_thrend_update() {
    block_list *l_list = p_blocklist;
    // be änderung Updaten
    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;

        SDL_LockMutex ( p_mutex_handle);
        if( l_node->isChanged()) {
            l_node->changed( false);
            SDL_UnlockMutex ( p_mutex_handle);
            if( l_list)
                l_node->updateArray( l_list);
            break;
        } else {
            SDL_UnlockMutex ( p_mutex_handle);
        }
        l_node = l_node->next;
    }

    int l_status_mutex = SDL_TryLockMutex( p_mutex_handle);

    if( l_status_mutex == 0) { // mutex lock for this tread
        while( p_update_changes.empty() == false) {
            // Get a reference to the front node.
            world_data_list &l_list_node = p_update_changes.front();

            glm::ivec3 l_position = l_list_node.position;
            Chunk *l_node = getChunk( l_position);
            bool l_landscape = l_list_node.landscape;

            p_update_changes.pop();

            SDL_LockMutex ( p_mutex_physic);
            if( l_node != NULL) {
                l_node->updateArray( l_list);
                //l_node->changed( false, true);
            }
            SDL_UnlockMutex ( p_mutex_physic);
        }
        SDL_UnlockMutex ( p_mutex_handle);
    }

    SDL_LockMutex ( p_mutex_handle);
    while( p_change_blocks.empty() == false) {
        // Get a reference to the front node.
        world_change_block &l_list_node = p_change_blocks.front();

        glm::ivec3 l_position = l_list_node.position;
        Chunk *l_node = l_list_node.chunk;
        int l_block_id = l_list_node.id;

        p_change_blocks.pop();

        SDL_LockMutex ( p_mutex_physic);
        setTile( l_node, l_position, l_block_id);
        SDL_UnlockMutex ( p_mutex_physic);
    }
    SDL_UnlockMutex ( p_mutex_handle);
}

void world::process_thrend_physic() {

    /*
    // Reset Idle time -> bis der Chunk sich selbst löscht
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL || p_physicScene == NULL)
            break;

        // create body
        if( node->createPhysicBody( p_physicScene, p_mutex_physic)) {
            break; // olny one
        }

        // next
        node = node->next;
    }*/

    // check if time pass
    while( (float)SDL_GetTicks() - p_time > WORLD_PHYSIC_FIXED_TIMESTEP*1000.f) {
        // p_time calculate
        p_time += ((float)WORLD_PHYSIC_FIXED_TIMESTEP*1000.f);

        if( getChunk( glm::ivec3( 0, 0, 0)) == NULL)
           return;

        for( object *l_object: p_objects) {
            l_object->addVelocity( p_gravity*WORLD_PHYSIC_FIXED_TIMESTEP);

            glm::vec3 l_collision_position = l_object->getPosition() + l_object->getVerlocity();

            for( int i = -1; i < fabs( l_object->getVerlocity().y)+1; i++) {
                //printf( "%.2f\n", l_object->getVerlocity().y);

                glm::ivec3 l_check_tile; // = glm::ivec3( l_collision_position.x / CHUNK_SIZE, , l_collision_position.z/ CHUNK_SIZE);

                l_check_tile.x = (float)l_collision_position.x;
                l_check_tile.y = l_collision_position.y + i;
                l_check_tile.z = (float)l_collision_position.z;

                Chunk *l_chunk = getChunk( glm::ivec3( 0, 0, 0) );

//                printf( "%d %d %d\n", (int)l_check_tile.x, (int)l_check_tile.y, (int)l_check_tile.z);
                if( l_chunk != NULL && l_chunk->getTile( l_check_tile) != EMPTY_BLOCK_ID ) {
                    if( physic::testAABB( l_collision_position + l_object->getType()->getHitbox() + l_object->getType()->getHitboxOffset(), l_object->getType()->getHitbox(), l_check_tile, glm::vec3( 1, 1, 1))) {
                        l_object->setVelocity( glm::vec3( 0, 0.0, 0));
                        break;
                    }
                }
            }


            l_object->process_phyisc();
        }
    }
}

void world::process_object_handling() {
    for( object *l_object: p_objects) {
        l_object->process( );
    }
}

void world::deleteChunks( Chunk* chunk) {
    if( chunk == NULL )
        return;
    if( chunk->next )
        deleteChunks( chunk->next );
    deleteChunk( chunk);
}

void world::deleteChunk( Chunk* node) {
    if( node == NULL)
        return;

    int pos_x = node->getPos().x;
    int pos_y = node->getPos().y;
    int pos_z = node->getPos().z;

    // löschen des chunks
    Chunk *tmp = p_chunk_start;
    Chunk *tmpOld = NULL;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->getPos().x == pos_x &&
             tmp->getPos().y == pos_y &&
             tmp->getPos().z == pos_z ) {
            if( tmpOld == NULL)
                p_chunk_start = tmp->next;
            else tmpOld->next = tmp->next;

            // delete pointer of sides
            if( node->getSide( CHUNK_SIDE_X_POS) )
                node->setSide( NULL, CHUNK_SIDE_X_POS);
            if( node->getSide( CHUNK_SIDE_X_NEG) )
                node->setSide( NULL, CHUNK_SIDE_X_NEG);
            if( node->getSide( CHUNK_SIDE_Y_POS) )
                node->setSide( NULL, CHUNK_SIDE_Y_POS);
            if( node->getSide( CHUNK_SIDE_Y_NEG) )
                node->setSide( NULL, CHUNK_SIDE_Y_NEG);
            if( node->getSide( CHUNK_SIDE_Z_POS) )
                node->setSide( NULL, CHUNK_SIDE_Z_POS);
            if( node->getSide( CHUNK_SIDE_Z_NEG) )
                node->setSide( NULL, CHUNK_SIDE_Z_NEG);

            // löschen
            delete tmp;
            // runterzählen
            p_chunk_amount--;
            break;
        }
        tmpOld = tmp;
        tmp = tmp->next;
    }
}

Chunk *world::createChunk( glm::ivec3 position) {
    Chunk *node;
    Chunk *l_side;

    int l_seed = atoi( p_name.c_str());

    // Chunk erstellen
    node = new Chunk( position, l_seed);
    node->next = NULL;

    p_chunk_amount++; // p_chunk_start mitzählen

    Chunk *tmp = p_chunk_start;
    int z = 0;

    // add chunk to the node
    if( p_chunk_start == NULL) {
        p_chunk_start = node;
    } else {
        while( tmp->next != NULL ) {
            tmp = tmp->next;
            z++;
        }
        tmp->next = node;
    }

    // set the sides
    l_side = getChunk( position + glm::ivec3( 1, 0, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_X_NEG);
        node->setSide( l_side, CHUNK_SIDE_X_POS);
    }
    l_side = getChunk( position + glm::ivec3(-1, 0, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_X_POS);
        node->setSide( l_side, CHUNK_SIDE_X_NEG);
    }
    l_side = getChunk( position + glm::ivec3( 0, 1, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Y_NEG);
        node->setSide( l_side, CHUNK_SIDE_Y_POS);
    }
    l_side = getChunk( position + glm::ivec3( 0,-1, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Y_POS);
        node->setSide( l_side, CHUNK_SIDE_Y_NEG);
    }
    l_side = getChunk( position + glm::ivec3( 0, 0, 1));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Z_NEG);
        node->setSide( l_side, CHUNK_SIDE_Z_POS);
    }
    l_side = getChunk( position + glm::ivec3( 0, 0,-1));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Z_POS);
        node->setSide( l_side, CHUNK_SIDE_Z_NEG);
    }

    return node;
}

bool world::CheckChunk( int X, int Y, int Z) {
    Chunk *tmp = p_chunk_start;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->getPos().x == X &&
             tmp->getPos().y == Y &&
             tmp->getPos().z == Z )
             return true;
        tmp = tmp->next;
    }
    return false;
}



Chunk* world::getChunk( glm::ivec3 position) {
    Chunk *tmp = p_chunk_start;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->getPos() == position )
             return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void world::addChunk( glm::ivec3 position, bool generateLandscape ) {
    world_data_list l_obj( position, generateLandscape);

    p_creatingList.push( l_obj);
}
void world::addDeleteChunk( glm::ivec3 position ) {
    world_data_list l_obj( position, false);
    p_deletingList.push( l_obj);
}

void world::drawObjects( graphic *graphic, Shader *shader) {
    for( object *l_object: p_objects) {
        l_object->draw( shader);
    }
}

void world::drawObjectsDebug( graphic *graphic, Shader *shader) {
    for( object *l_object: p_objects) {
        l_object->draw_debug( shader);
    }
}

void world::drawVoxel( graphic *graphic, Shader *shader) {

    shader->setSize( (graphic->getDisplay()->getTilesetWidth()/16), ( graphic->getDisplay()->getTilesetHeight()/16) );

    p_blocklist->getTilemapTexture()->Bind();

    // draw node
    p_chunk_visible_amount = 0;
    drawNode( shader);
}

void world::drawNode( Shader* shader) {
    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        if( l_node->draw( shader) )
            p_chunk_visible_amount++;

        // next
        l_node = l_node->next;
    }
}

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

bool world::fileExists(std::string filename) {
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

void world::save() {
    Timer l_time;
    l_time.Start();
    int l_amount = 0;
    std::string l_folder = "worldsave/" + p_name;
    mkdir( l_folder.c_str());


    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_amount++;
        std::ofstream l_file( l_folder + "/" + patch::to_string( l_node->getPos().x) + "_" + patch::to_string( l_node->getPos().y)+ "_" + patch::to_string( l_node->getPos().z)+ "_" + ".schunk", std::ios::binary);

        l_node->save( &l_file);

        // next
        l_node = l_node->next;
    }

    printf( "world::save %dms\n", l_time.GetTicks());
}

bool world::load() {
    Timer l_time;
    l_time.Start();

    DIR* l_handle;
    struct dirent* l_dirEntry;
    bool l_found_once = false;
    std::string l_name;
    std::string l_filepath;
    std::string l_path = "worldsave/" + p_name + "/";

    // open folder
    l_handle = opendir( l_path.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            l_name = l_dirEntry->d_name;
            l_filepath =  l_path + l_name;
            if( fileExists( l_filepath )) {
                load_chunk( l_filepath);
                l_found_once = true;
            }
        }
    } else {
        printf( "world::load can't open the \"%s\" folder\n", l_path.c_str());
    }
    closedir( l_handle );

    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_node->changed( true);
        l_node = l_node->next;
    }

    printf( "world::load %dms \"%s\" folder\n", l_time.GetTicks(), l_path.c_str());

    if( l_found_once)
        return true;
    return false;
}


void world::load_chunk( std::string path_file) {
    std::ifstream l_file( path_file, std::ios::binary);
    glm::ivec3 l_position;

    l_file.read ((char*)&l_position.x, sizeof ( glm::int_t));
    l_file.read ((char*)&l_position.y, sizeof ( glm::int_t));
    l_file.read ((char*)&l_position.z, sizeof ( glm::int_t));

    Chunk *l_node = getChunk( l_position);
    if( l_node == NULL)
        l_node = createChunk( l_position);

    l_node->load( &l_file);
}
