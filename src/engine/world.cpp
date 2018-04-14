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

world::world( texture *image, block_list* B_List) {
    p_buysvector = false;
    p_chunk_amount = 0;
    p_chunk_start = NULL;
    p_chunk_last = NULL;
    p_world_tree_empty = true;
    p_blocklist = B_List;
    p_destroy = false;
    p_image = image;
    p_physicScene = NULL;

    // thrends
    p_mutex = SDL_CreateMutex ();
    p_mutex_physic = SDL_CreateMutex ();
    p_thread_handle = SDL_CreateThread( world_thread_handle, "world_thread_handle", (void *)this);
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        p_thread_update[i] = SDL_CreateThread( world_thread_update, "world_thread_update", (void *)this);
    p_thread_physic = SDL_CreateThread( world_thread_physic, "world_thread_physic", (void *)this);

    // physic
    p_physicScene = new b3World();
    p_physicScene->SetGravity( b3Vec3(0.0f, -9.8f, 0.0f));
    p_time = SDL_GetTicks();

    //p_renderer.init();
}

world::~world() {
    int l_return;
    p_destroy = true;
    for( int i = 0; i < WORLD_UPDATE_THRENDS; i++)
        SDL_WaitThread( p_thread_update[i], &l_return);
    SDL_WaitThread( p_thread_handle, &l_return);
    SDL_WaitThread( p_thread_physic, &l_return);
    SDL_DestroyMutex( p_mutex);
    SDL_DestroyMutex( p_mutex_physic);

    // Löschen der Welt
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

void world::SetTile( Chunk *chunk, int tile_x, int tile_y, int tile_z, int ID) {
    if( chunk == NULL) {
        printf( "SetTile: Cant set tile!\n");
        return;
    }
    glm::vec3 l_pos = chunk->getPos() * glm::vec3( CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

    chunk->set( tile_x-l_pos.x, tile_y-l_pos.y, tile_z-l_pos.z, ID);
}

void world::process_thrend_handle() {
    for( int i = 0; i < (int)p_creatingList.size(); i++)
    {
        Chunk *l_node = getChunk( p_creatingList[i].position);
        if( l_node == NULL) {
            SDL_LockMutex ( p_mutex);
            createChunk( p_creatingList[i].position.x, p_creatingList[i].position.y, p_creatingList[i].position.z, p_creatingList[i].landscape);
            SDL_UnlockMutex ( p_mutex);
            p_creatingList.erase( p_creatingList.begin()+ i);

            break;
        }
    }

    for( auto l_pos:p_deletingList)
    {
        Chunk *l_node = getChunk( l_pos.position);
        if( l_node != NULL) {
            SDL_LockMutex ( p_mutex);
            deleteChunk( l_node);
            SDL_UnlockMutex ( p_mutex);
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

        SDL_LockMutex ( p_mutex);
        if( l_node->isChanged()) {
            l_node->changed( false);
            SDL_UnlockMutex ( p_mutex);
            if( l_list)
                l_node->updateArray( l_list);
        } else {
            SDL_UnlockMutex ( p_mutex);
        }

        l_node = l_node->next;
    }
}

void world::process_thrend_physic() {
    // Reset Idle time -> bis der Chunk sich selbst löscht
    Chunk *node = p_chunk_start;
    for( ;; ) {
        if( node == NULL || p_physicScene == NULL)
            break;

        // create body
        SDL_LockMutex ( p_mutex);
        if( node->createPhysicBody( p_physicScene)) {
            SDL_UnlockMutex ( p_mutex);
            break; // olny one
        }
        SDL_UnlockMutex ( p_mutex);

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
        SDL_LockMutex ( p_mutex);
        if( p_physicScene)
            p_physicScene->Step( WORLD_PHYSIC_FIXED_TIMESTEP, velocityIterations, positionIterations);
        SDL_UnlockMutex ( p_mutex);
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

            // chunk seiten löschen
            Chunk *snode;
            if( CheckChunk( pos_x+1, pos_y, pos_z)) {
                snode = getChunk( glm::vec3(pos_x+1, pos_y, pos_z));
                snode->back = NULL;
            }
            if( CheckChunk( pos_x-1, pos_y, pos_z)) {
                snode = getChunk( glm::vec3(pos_x-1, pos_y, pos_z));
                snode->front = NULL;
            }
            if( CheckChunk( pos_x, pos_y+1, pos_z)) {
                snode = getChunk( glm::vec3(pos_x, pos_y+1, pos_z));
                snode->down = NULL;
            }
            if( CheckChunk( pos_x, pos_y-1, pos_z)) {
                snode = getChunk( glm::vec3(pos_x, pos_y-1, pos_z));
                snode->up = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z+1)) {
                snode = getChunk( glm::vec3(pos_x, pos_y, pos_z+1));
                snode->left = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z-1)) {
                snode = getChunk( glm::vec3(pos_x, pos_y, pos_z-1));
                snode->right = NULL;
            }
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

    // Chunk erstellen
    node = new Chunk( pos_x, pos_y, pos_z, 102457);
    node->next = NULL;

    if( generateLandscape)
        Landscape_Generator( node, p_blocklist);

    p_chunk_amount++; // p_chunk_start mitzählen

    Chunk *tmp = p_chunk_start;
    int z = 0;

    // Falls hauptzweik nicht exestiert erstllen
    if( p_chunk_start == NULL) {
        p_chunk_start = node;
    } else {
        while( tmp->next != NULL ) {
            tmp = tmp->next;
            z++;
        }
        tmp->next = node;
    }

    // finish -> go to update section
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

void world::draw( graphic *graphic, Shader *shader, glm::mat4 view, glm::mat4 projection) {

    shader->setSize( (graphic->getDisplay()->getTilesetWidth()/16), ( graphic->getDisplay()->getTilesetHeight()/16) );

    p_image->Bind();

    // draw node
    drawNode( shader, view, projection);
}

void world::drawNode( Shader* shader, glm::mat4 view, glm::mat4 projection) {
    Chunk *l_node = p_chunk_start;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_node->draw( shader, view, projection);

        // next
        l_node = l_node->next;
    }
}
