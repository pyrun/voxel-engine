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

world::world( block_list* block_list) {
    p_buysvector = false;
    p_chunk_amount = 0;
    p_chunk_visible_amount = 0;
    p_chunk_start = NULL;
    p_chunk_last = NULL;
    p_world_tree_empty = true;
    p_blocklist = block_list;
    p_destroy = false;
    p_physicScene = NULL;
    p_time = SDL_GetTicks();

    // creating two mutex
    p_mutex_handle = SDL_CreateMutex();
    p_mutex_physic = SDL_CreateMutex();

    // creating the threads
    p_thread_handle = SDL_CreateThread( world_thread_handle, "world_thread_handle", (void *)this);
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        p_thread_update[i] = SDL_CreateThread( world_thread_update, "world_thread_update", (void *)this);
    p_thread_physic = SDL_CreateThread( world_thread_physic, "world_thread_physic", (void *)this);

    // creating physic handle
    p_physicScene = new b3World();
    p_physicScene->SetGravity( b3Vec3(0.0f, -9.8f, 0.0f));
}

world::~world() {
    int l_return; // didnt use

    // wait for thread end
    p_destroy = true;
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        SDL_WaitThread( p_thread_update[i], &l_return);
    SDL_WaitThread( p_thread_handle, &l_return);
    SDL_WaitThread( p_thread_physic, &l_return);

    // destroy the mutex
    SDL_DestroyMutex( p_mutex_handle);
    SDL_DestroyMutex( p_mutex_physic);

    // free ram
    deleteChunks( p_chunk_start);
    while( p_chunk_amount != 0) {
        SDL_Delay(1);
    }
}

int world::GetTile( int x, int y, int z) {
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL)
            break;
        int p_chunk_x = x-node->getPos().x*CHUNK_SIZE;
        int p_chunk_y = y-node->getPos().y*CHUNK_SIZE;
        int p_chunk_z = z-node->getPos().z*CHUNK_SIZE;

        // X - Achse
        if(p_chunk_x < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_x >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if(p_chunk_y < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_y >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if(p_chunk_z < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_z >= CHUNK_SIZE) {
            node = node->next;
            continue;
        }

        int l_tile = node->getTile( p_chunk_x, p_chunk_y, p_chunk_z );
        if( l_tile == EMPTY_BLOCK_ID)
            break;
        return l_tile;
    }
    return NULL;
}

Chunk* world::getChunkWithPos( int x, int y, int z) {
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL)
            break;
        int p_chunk_x = x-node->getPos().x*CHUNK_SIZE;
        int p_chunk_y = y-node->getPos().y*CHUNK_SIZE;
        int p_chunk_z = z-node->getPos().z*CHUNK_SIZE;

        // X - Achse
        if(p_chunk_x < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_x >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if(p_chunk_y < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_y >= CHUNK_SIZE ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if(p_chunk_z < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_z >= CHUNK_SIZE) {
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
    p_change_blocks.push_back( l_change);
}

void world::setTile( Chunk *chunk, glm::vec3 position, int id) {
    if( chunk == NULL) {
        printf( "world::setTile: chunk is defined as NULL\n");
        return;
    }
    glm::vec3 l_chunk_position = chunk->getPos() * glm::vec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

    position = position - l_chunk_position;

    chunk->set( position, id, false);

    //chunk->setSunlight( position, 0);

    calcSunRay( chunk, position);

    if( (int)position.x == 0 && chunk->getSide( CHUNK_SIDE_X_NEG))
        chunk->getSide( CHUNK_SIDE_X_NEG)->changed( true);
    if( (int)position.x == CHUNK_SIZE-1 && chunk->getSide( CHUNK_SIDE_X_POS))
        chunk->getSide( CHUNK_SIDE_X_POS)->changed( true);
    if( (int)position.z == 0 && chunk->getSide( CHUNK_SIDE_Z_NEG))
        chunk->getSide( CHUNK_SIDE_Z_NEG)->changed( true);
    if( (int)position.z == CHUNK_SIZE-1 && chunk->getSide( CHUNK_SIDE_Z_POS))
        chunk->getSide( CHUNK_SIDE_Z_POS)->changed( true);

    chunk->changed( true);
}

void world::calcSunRay( Chunk *chunk, glm::vec3 position, bool firstBlock) {
    Chunk *l_chunk = chunk;
    glm::vec3 l_position = position;
    int l_ligthing = 15;
    /*while( l_chunk->getSide( CHUNK_SIDE_Y_POS) != NULL)
        l_chunk = l_chunk->getSide( CHUNK_SIDE_Y_POS);*/

    l_position.y = CHUNK_SIZE;
    for( ;;) {
        l_position.y = l_position.y-1;
        if( l_position.y < 0) {
            l_chunk = l_chunk->getSide( CHUNK_SIDE_Y_NEG);
            l_position.y = CHUNK_SIZE;
        }
        if( l_chunk == NULL)
            return;

        l_chunk->setSunlight( l_position, l_ligthing);
        // hit ground?
        if( l_chunk->checkTile( l_position - glm::vec3( 0, 1, 0) ) ) {
            l_ligthing = 0;
            //l_chunk->set( l_position, 5, false);
        }

    }
}

void world::process_thrend_handle() {
    for( int i = 0; i < (int)p_creatingList.size(); i++)
    {
        Chunk *l_node = getChunk( p_creatingList[i].position);
        if( l_node == NULL) {
            SDL_LockMutex ( p_mutex_handle);
            createChunk( p_creatingList[i].position.x, p_creatingList[i].position.y, p_creatingList[i].position.z, p_creatingList[i].landscape);
            SDL_UnlockMutex ( p_mutex_handle);
            p_creatingList.erase( p_creatingList.begin()+ i);
            break;
        }
    }

    for( auto l_pos:p_deletingList)
    {
        Chunk *l_node = getChunk( l_pos.position);
        if( l_node != NULL) {
            SDL_LockMutex ( p_mutex_handle);
            SDL_LockMutex ( p_mutex_physic);
            deleteChunk( l_node);
            SDL_UnlockMutex ( p_mutex_handle);
            SDL_UnlockMutex ( p_mutex_physic);
        }
    }
    p_deletingList.clear();
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
            SDL_LockMutex ( p_mutex_physic);
            if( l_list)
                l_node->updateArray( l_list);
            break;
            SDL_UnlockMutex ( p_mutex_physic);
        } else {
            SDL_UnlockMutex ( p_mutex_handle);
        }
        l_node = l_node->next;
    }

    SDL_LockMutex ( p_mutex_handle);
    for( int i = 0; i < (int)p_change_blocks.size(); i++) {
        SDL_LockMutex ( p_mutex_physic);
        setTile( p_change_blocks[i].chunk, p_change_blocks[i].position, p_change_blocks[i].id);
        SDL_UnlockMutex ( p_mutex_physic);
    }
    p_change_blocks.clear();
    SDL_UnlockMutex ( p_mutex_handle);
}

void world::process_thrend_physic() {
    // Reset Idle time -> bis der Chunk sich selbst löscht
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL || p_physicScene == NULL)
            break;

        // create body
        SDL_LockMutex ( p_mutex_physic);
        if( node->createPhysicBody( p_physicScene)) {
            SDL_UnlockMutex ( p_mutex_physic);
            break; // olny one
        }
        SDL_UnlockMutex ( p_mutex_physic);

        // next
        node = node->next;
    }

    // check if time pass
    while( (float)SDL_GetTicks() - p_time > WORLD_PHYSIC_FIXED_TIMESTEP*1000.f) {
        // p_time calculate
        p_time += ((float)WORLD_PHYSIC_FIXED_TIMESTEP*1000.f);
        const u32 velocityIterations = 8; // Number of iterations for the velocity constraint solver.
        const u32 positionIterations = 2; // Number of iterations for the position constraint solver.
        // fixed step
        SDL_LockMutex ( p_mutex_physic);
        if( p_physicScene)
            p_physicScene->Step( WORLD_PHYSIC_FIXED_TIMESTEP, velocityIterations, positionIterations);
        SDL_UnlockMutex ( p_mutex_physic);
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

Chunk *world::createChunk( int pos_x, int pos_y, int pos_z, bool generateLandscape, bool update) {
    Chunk *node;
    Chunk *l_side;
    glm::vec3 l_pos = glm::vec3( pos_x, pos_y, pos_z);

    // Chunk erstellen
    node = new Chunk( l_pos.x, l_pos.y, l_pos.z, 102457);
    node->next = NULL;

    if( generateLandscape)
        Landscape_Generator( node, p_blocklist);

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
    l_side = getChunk( l_pos + glm::vec3( 1, 0, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_X_NEG);
        node->setSide( l_side, CHUNK_SIDE_X_POS);
    }
    l_side = getChunk( l_pos + glm::vec3(-1, 0, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_X_POS);
        node->setSide( l_side, CHUNK_SIDE_X_NEG);
    }
    l_side = getChunk( l_pos + glm::vec3( 0, 1, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Y_NEG);
        node->setSide( l_side, CHUNK_SIDE_Y_POS);
    }
    l_side = getChunk( l_pos + glm::vec3( 0,-1, 0));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Y_POS);
        node->setSide( l_side, CHUNK_SIDE_Y_NEG);
    }
    l_side = getChunk( l_pos + glm::vec3( 0, 0, 1));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Z_NEG);
        node->setSide( l_side, CHUNK_SIDE_Z_POS);
    }
    l_side = getChunk( l_pos + glm::vec3( 0, 0,-1));
    if( l_side) {
        l_side->setSide( node, CHUNK_SIDE_Z_POS);
        node->setSide( l_side, CHUNK_SIDE_Z_NEG);
    }

    // set flag "need update"
    if( update)
        node->changed( true);

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



Chunk* world::getChunk( glm::vec3 position) {
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

void world::addChunk( glm::tvec3<int> pos, bool generateLandscape ) {
    world_data_list l_obj;

    l_obj.position = pos;
    l_obj.landscape = generateLandscape;

    p_creatingList.push_back( l_obj);
}
void world::addDeleteChunk( glm::tvec3<int> pos ) {
    world_data_list l_obj;

    l_obj.position = pos;
    l_obj.landscape = false;

    p_deletingList.push_back( l_obj);
}

void world::draw( graphic *graphic, Shader *shader) {

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
