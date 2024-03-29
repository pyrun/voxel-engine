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

world::world( block_list* block_list, std::string name, object_handle *objectHandle, bool player) {
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
    p_gravity = glm::vec3( 0, -150.0/1000., 0);
    p_player_world = player;
    p_object_sync = 200; // ms

    // calls
    changeCall = NULL;
    createObjectCall = NULL;
    objectSyncCall = NULL;
    deleteObjectCall = NULL;

    // creating two mutex
    p_mutex_handle = SDL_CreateMutex();
    p_mutex_physic = SDL_CreateMutex();

    // creating the threads
    for( int i = 0; i < WORLD_HANDLE_THRENDS; i++)
        p_thread_handle[i] = SDL_CreateThread( world_thread_handle, "world_thread_handle", (void *)this);
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        p_thread_update[i] = SDL_CreateThread( world_thread_update, "world_thread_update", (void *)this);

    // timer
    p_snyc_object_timer.start();

    printf( "world::world \"%s\" created\n", getName().c_str());
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

int world::createObject( std::string name, glm::vec3 position, unsigned int id, bool call) {
    object *l_object;

    // set target myself
    lua_object_set_targets( this);

    // get type
    object_type *l_type = p_pointer_object_handle->get( name);
    if( !l_type) {
        printf( "world::createObject type \"%s\"not found\n", name.c_str());
        return -1;
    }

    // now create object
    l_object = new object();
    l_object->setPosition( position);

    // set id
    if( id == 0) {
        l_object->setId( ++p_object_id);
    } else {
        l_object->setId( id);
        p_object_id = id;
    }

    // puch back
    p_objects.push_back( l_object);

    //
    printf( "world::createObject \"%s\" #%d %.1fx %.1fy %.1fz\n", name.c_str(), p_object_id, position.x, position.y, position.z);

    // set now type ( calls lua and install it)
    l_object->setType( l_type);

    // call function ( network)
    if( createObjectCall != NULL && call)
        createObjectCall( this, name, position, p_object_id);

    return p_object_id;
}

object *world::getObject( int id) {
    for( object *l_object:p_objects)
        if( l_object->getId() == id)
            return l_object;
    return NULL;
}

void world::deleteObject( int id) {
    for( int i = 0; i < (int)p_objects.size(); i++) {
        if( p_objects[i]->getId() == id) {
            p_objects.erase( p_objects.begin() + i);
            return;
        }
    }
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

void world::changeBlock( Chunk *chunk, glm::vec3 position, int id, bool call) {
    world_change_block l_change;
    l_change.position = position;
    l_change.id = id;
    l_change.chunk = chunk;

    p_change_blocks.push( l_change);

    if( changeCall && call)
        changeCall( this, chunk, position, id);
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

    // add too queue
    chunk->setTorchlight( l_position, value);
    SDL_LockMutex ( p_mutex_handle);
    p_lightsAdd.emplace( l_position, chunk);
    SDL_UnlockMutex ( p_mutex_handle);
}

void world::delTorchlight( Chunk *chunk, glm::ivec3 position) {
    int l_light;

    // calculate the position by chunk
    glm::ivec3 l_chunk_position = chunk->getPos() * glm::ivec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
    position = position - l_chunk_position;

    // add too queue
    l_light = chunk->getTorchlight( position);
    chunk->setTorchlight( position, 0);
    p_lightsDel.emplace( position, chunk, l_light);
}

void world::caluculationLight() {
    Chunk *l_chunk = p_chunk_start;
    for( ;; ) {
        if( l_chunk == NULL)
            break;

        // get every block
        for( int x = 0; x < CHUNK_SIZE; x++) {
            for( int y = 0; y < CHUNK_SIZE; y++) {
                for( int z = 0; z < CHUNK_SIZE; z++) {
                    block *l_block = p_blocklist->get( l_chunk->getTile( x, y, z));
                    if( l_block && l_block->isLight())
                        addTorchlight( l_chunk, glm::ivec3( x, y, z) + l_chunk->getPos()*CHUNK_SIZE, l_block->getLighting());
                }
            }
        }

        l_chunk = l_chunk->next;
    }
}

void world::process_thrend_handle() {
    landscape_return *l_return;
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
                if( p_player_world)
                    l_return = p_landscape_generator->getPlayerGenerator( l_node)->generator( l_node, p_blocklist);
                else
                    l_return = p_landscape_generator->getWorldGenerator( l_node)->generator( l_node, p_blocklist);
                l_lights = l_return->lights;
                l_node->changed( true);

                // set spawn point
                if( l_return->spawn_point != glm::ivec3( 0, 0, 0)) {
                    setSpawnPoint( l_return->spawn_point + l_node->getPos() * CHUNK_SIZE );
                }

                //SDL_UnlockMutex ( p_mutex_handle);
                delete l_return;
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
    // be �nderung Updaten
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
    bool l_hit;
    glm::ivec3 l_collision_block;
    glm::vec3 l_collision_position;
    glm::vec3 l_shift_position = glm::vec3( 0, 0, 0);
    Chunk *l_chunk;
    glm::vec3 l_size_block = glm::vec3( 1, 1, 1);
    glm::vec3 l_hitbox;

    // get start_chunk for get block ( no good solution)
    l_chunk = getChunk( glm::ivec3( 0, 0, 0) );

    // check if time pass
    while( (float)SDL_GetTicks() - p_time > WORLD_PHYSIC_FIXED_TIMESTEP*1000.f) {
        // time calculate for next pass
        p_time += ((float)WORLD_PHYSIC_FIXED_TIMESTEP*1000.f);

        p_physic_flag = true;
        if( l_chunk == NULL)
           return;

        // calculate each object
        for( object *l_object: p_objects) {
            // reset hit sides and add gravity
            l_hit = false;
            l_object->addVelocity( p_gravity*WORLD_PHYSIC_FIXED_TIMESTEP*l_object->getGravityForce());

            l_hitbox = l_object->getType()->getHitbox();
            l_collision_position = l_object->getPosition() + l_object->getVerlocity();

            // set shift table
            glm::vec3 l_shift[6] = {glm::vec3( 0, 0, 0),
                                    glm::vec3( l_collision_position.x>0?l_hitbox.x + 0.5f:-l_hitbox.x - 0.5f, 0, 0),
                                    glm::vec3( l_collision_position.x>0?l_hitbox.x + 0.5f:-l_hitbox.x - 0.5f, 0, l_collision_position.z>0?l_hitbox.z + 0.5f:-l_hitbox.z - 0.5f),
                                    glm::vec3( 0, 0, l_collision_position.z>0?l_hitbox.z + 0.5f:-l_hitbox.z - 0.5f),
                                    glm::vec3( 0,0,0),
                                    glm::vec3( 0,0,0)};

            l_collision_block.x = (int)l_collision_position.x;
            l_collision_block.y = (int)l_collision_position.y;
            l_collision_block.z = (int)l_collision_position.z;

            for( int i = -2; i < fabs( l_object->getVerlocity().y)+1 && l_hit == false; i++) {
                for( int shift = 0; shift < 4 && l_hit == false; shift++) {
                    // set y
                    l_collision_block.x = (int)l_collision_position.x + (int)l_shift[ shift].x;
                    l_collision_block.y = (int)l_collision_position.y - i;
                    l_collision_block.z = (int)l_collision_position.z + (int)l_shift[ shift].z;

                    // check now tile
                    if( l_chunk != NULL && l_chunk->getTile( l_collision_block) != EMPTY_BLOCK_ID ) {
                        if( physic::testAABB2( l_collision_position, l_hitbox, l_collision_block, l_size_block) ) {
                            bool l_change_position = false;
                            // check with side hit
                            if( l_object->getVerlocity().y < 0 ) {
                                if( fabs( l_collision_block.y+l_size_block.y - l_collision_position.y) < 0.001f + fabs( l_object->getVerlocity().y) ) {
                                    l_object->setPositionY( l_collision_block.y+l_size_block.y + 0.00f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::ground, true);
                                }
                            } else {
                                if( fabs( l_collision_block.y-l_hitbox.y - l_collision_position.y) < 0.001f + fabs( l_object->getVerlocity().y) ) {
                                    l_object->setPositionY( l_collision_block.y-l_hitbox.y - 0.0f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::top, true);
                                }
                            }

                            // register the hit
                            if( l_change_position) {
                                l_object->setVelocityY( 0.0f);
                                l_hit = true;
                            }
                        }
                    }
                }
            }

            /// x collision
            l_hit = false;

            l_collision_position = l_object->getPosition() + glm::vec3( l_object->getVerlocity().x, 0, 0);

            l_collision_block.x = (int)l_collision_position.x;
            l_collision_block.y = (int)l_collision_position.y;
            l_collision_block.z = (int)l_collision_position.z;

            l_shift[0] = glm::vec3( 0.0f, 0.0f, 0.0f);
            l_shift[1] = glm::vec3( 0.0f, 0.0f, l_collision_position.z>0?l_hitbox.z + 0.5f:-l_hitbox.z - 0.5f);
            l_shift[2] = glm::vec3( 0.0f, l_hitbox.y, 0.0f);
            l_shift[3] = glm::vec3( 0.0f, l_hitbox.y, l_collision_position.z>0?l_hitbox.z + 0.5f:-l_hitbox.z - 0.5f);
            l_shift[4] = glm::vec3( 0.0f, l_hitbox.y + 0.5f, 0.0f);
            l_shift[5] = glm::vec3( 0.0f, l_hitbox.y + 0.5f, l_collision_position.z>0?l_hitbox.z + 0.5f:-l_hitbox.z - 0.5f);



            if(l_object->getPosition().y < 0 ) {
                for( int shift = 0; shift < 6; shift++) {
                    l_shift[ shift].y -= 1.0f;
                }
                l_shift_position.y = +0.01f;
            } else {
                l_shift_position.y = 0;
            }

            for( int i = -2; i < fabs( l_object->getVerlocity().x)+2 && l_hit == false; i++) {
                for( int shift = 0; shift < 6 && l_hit == false; shift++) {
                    l_collision_position = l_object->getPosition() + glm::vec3( l_object->getVerlocity().x, 0, 0) + l_shift_position;
                    // set collision position block
                    l_collision_block.x = (int)l_collision_position.x - i;
                    l_collision_block.y = (int)l_collision_position.y + (int)l_shift[ shift].y;
                    l_collision_block.z = (int)l_collision_position.z + (int)l_shift[ shift].z;

                    // check now tile
                    if( l_chunk != NULL && l_chunk->getTile( l_collision_block) != EMPTY_BLOCK_ID ) {
                        bool l_change_position = false;
                        if( physic::testAABB2( l_collision_position, l_hitbox, l_collision_block, l_size_block)) {
                            if( l_object->getVerlocity().x < 0) {
                                if( fabs( l_collision_block.x+l_size_block.x - l_collision_position.x) < 0.001f + fabs( l_object->getVerlocity().x) ) {
                                    l_object->setPositionX( l_collision_block.x+l_size_block.x + 0.001f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::east, true);
                                }
                            } else {
                                if( fabs( l_collision_block.x-l_hitbox.x - l_collision_position.x) < 0.001f + fabs( l_object->getVerlocity().x) ) {
                                    l_object->setPositionX( l_collision_block.x-l_hitbox.x - 0.001f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::west, true);
                                }
                            }

                            // register the hit
                            if( l_change_position) {
                                l_object->setVelocityX( 0.0 );
                                l_hit = true;
                            }
                        }
                    }
                }
            }

            /// z collision
            l_hit = false;

            l_collision_position = l_object->getPosition() + glm::vec3( 0, 0, l_object->getVerlocity().z);

            l_collision_block.x = (int)l_collision_position.x;
            l_collision_block.y = (int)l_collision_position.y;
            l_collision_block.z = (int)l_collision_position.z;

            l_shift[0] = glm::vec3( 0.0f, 0.0f, 0.0f);
            l_shift[1] = glm::vec3( l_collision_position.x>0?l_hitbox.x + 0.5f:-l_hitbox.x - 0.5f, 0.0f, 0.0f);
            l_shift[2] = glm::vec3( 0.0f, l_hitbox.y, 0);
            l_shift[3] = glm::vec3( l_collision_position.x>0?l_hitbox.x + 0.5f:-l_hitbox.x - 0.5f, l_hitbox.y, 0.0f);
            l_shift[4] = glm::vec3( 0.0f, l_hitbox.y + 0.5f, 0);
            l_shift[5] = glm::vec3( l_collision_position.x>0?l_hitbox.x + 0.5f:-l_hitbox.x - 0.5f, l_hitbox.y + 0.5f, 0.0f);

            if(l_object->getPosition().y < 0 ) {
                for( int shift = 0; shift < 6; shift++) {
                    l_shift[ shift].y -= 1.0f;
                }
                l_shift_position.y = +0.01f;
            } else {
                l_shift_position.y = 0;
            }

            for( int i = -2; i < fabs( l_object->getVerlocity().z)+2 && l_hit == false; i++) {
                for( int shift = 0; shift < 6 && l_hit == false; shift++) {
                    l_collision_position = l_object->getPosition() + glm::vec3( 0, 0, l_object->getVerlocity().z) + l_shift_position;

                    // set collision position block
                    l_collision_block.x = (int)l_collision_position.x + (int)l_shift[ shift].x;
                    l_collision_block.y = (int)l_collision_position.y + (int)l_shift[ shift].y;
                    l_collision_block.z = (int)l_collision_position.z - i;

                    // check now tile
                    if( l_chunk != NULL && l_chunk->getTile( l_collision_block) != EMPTY_BLOCK_ID ) {
                        if( physic::testAABB2( l_collision_position, l_hitbox, l_collision_block, l_size_block)) {
                            bool l_change_position = false;
                            if( l_object->getVerlocity().z < 0) {
                                if( fabs( l_collision_block.z+l_size_block.z - l_collision_position.z) < 0.001f + fabs( l_object->getVerlocity().z) ) {
                                    l_object->setPositionZ( l_collision_block.z+l_size_block.z + 0.001f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::south, true);
                                }
                            } else {
                                if( fabs( l_collision_block.z-l_hitbox.z - l_collision_position.z) < 0.001f + fabs( l_object->getVerlocity().z) ) {
                                    l_object->setPositionZ( l_collision_block.z-l_hitbox.z - 0.001f);
                                    l_change_position = true;
                                    l_object->setHit( physic::hit_side::north, true);
                                }
                            }

                            // register the hit
                            if( l_change_position) {
                                l_object->setVelocityZ( 0.0f);

                                l_hit = true;
                            }
                        }
                    }
                }
            }

            if( l_object->getHit( physic::ground)) {
                l_object->setVelocityX( l_object->getVerlocity().x*0.8f);
                l_object->setVelocityZ( l_object->getVerlocity().z*0.8f);
            }

            // we finish with block collision
            l_object->process_phyisc();
        }
    }
}

void world::process_object_handling() {
    bool p_sync = false;

    // check for next sync
    if ( p_snyc_object_timer.getTicks() > WORLD_OBJECT_SYNC_MIN + p_object_sync) {
        p_sync = true;
        p_snyc_object_timer.start();
    }

    // each object call process
    for( object *l_object: p_objects) {
        l_object->process();
        if( p_sync && objectSyncCall != NULL)
            objectSyncCall( this, l_object);
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

    // l�schen des chunks
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

            // l�schen
            delete tmp;
            // runterz�hlen
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

    p_chunk_amount++; // p_chunk_start mitz�hlen

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

void world::reloadScripts() {
    for( auto l_object:p_objects)
        l_object->reloadScript();
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
    timer l_time;
    l_time.start();
    int l_amount = 0;
    std::string l_folder = "worldsave/" + p_name;
    std::string l_folder_chunks = l_folder + "/chunks";
    mkdir( "worldsave/");
    mkdir( l_folder.c_str());
    mkdir( l_folder_chunks.c_str());

    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_amount++;
        std::ofstream l_file( l_folder_chunks + "/" + patch::to_string( l_node->getPos().x) + "_" + patch::to_string( l_node->getPos().y)+ "_" + patch::to_string( l_node->getPos().z)+ "_" + ".schunk", std::ios::binary);

        l_node->save( &l_file);

        // next
        l_node = l_node->next;
    }

    config *l_config = new config( l_folder + "/" + "config.xml");
    l_config->set( "spawn_point_x", patch::to_string( getSpawnPoint().x), "world");
    l_config->set( "spawn_point_y", patch::to_string( getSpawnPoint().y), "world");
    l_config->set( "spawn_point_z", patch::to_string( getSpawnPoint().z), "world");
    delete l_config;

    printf( "world::save %dms\n", l_time.getTicks());
}

bool world::load() {
    timer l_time;
    l_time.start();

    DIR* l_handle;
    struct dirent* l_dirEntry;
    bool l_found_once = false;
    std::string l_name;
    std::string l_filepath;
    std::string l_path = "worldsave/" + p_name + "/";
    std::string l_folder_chunks = l_path + "/chunks/";

    // open folder
    l_handle = opendir( l_folder_chunks.c_str());
    if ( l_handle != NULL ) {
        while ( 0 != ( l_dirEntry = readdir( l_handle ) ))  {
            l_name = l_dirEntry->d_name;
            l_filepath =  l_folder_chunks + l_name;
            if( fileExists( l_filepath )) {
                load_chunk( l_filepath);
                l_found_once = true;
            }
        }
    } else {
        printf( "world::load can't open the \"%s\" folder\n", l_folder_chunks.c_str());
    }
    closedir( l_handle );

    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_node->changed( true);
        l_node = l_node->next;
    }

    config *l_config = new config( l_path + "/" + "config.xml");
    glm::vec3 l_spawn =  glm::vec3( atof( l_config->get( "spawn_point_x", "world", "0").c_str() ),
                                    atof( l_config->get( "spawn_point_y", "world", "0").c_str() ),
                                    atof( l_config->get( "spawn_point_z", "world", "0").c_str() ));
    setSpawnPoint( l_spawn);
    delete l_config;

    printf( "world::load %dms \"%s\" folder\n", l_time.getTicks(), l_path.c_str());

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
