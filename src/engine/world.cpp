#include "world.h"
#include <time.h>
#include "../system/timer.h"

static int world_thread(void *data)
{
    int cnt;

    for ( ;; ) {
        world *l_world = (world*)data;
        l_world->process_thrend();

        if( l_world->getDestory() )
            break;

        SDL_Delay(1);
    }

    return 0;
}

world::world( texture *image, block_list* B_List, bool clear) {
    p_seed = (int)time(NULL); // p_seed
    p_buysvector = false;
    p_chunk_amount = 0;
    Chunks = NULL;
    p_world_tree_empty = true;
    p_blocklist = B_List;
    p_destroy = false;
    p_image = image;
    p_clear = clear;

    p_thread = SDL_CreateThread(world_thread, "world_thread", (void *)this);
}

world::~world() {
    int l_return;
    p_destroy = true;
    SDL_WaitThread( p_thread, &l_return);

    // Löschen der Welt
    deleteChunks( Chunks);
    while( p_chunk_amount != 0) {
        SDL_Delay(1);
    }
}

tile* world::GetTile( int x, int y, int z) {
    Chunk *node = Chunks;
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

        tile *l_tile = node->getTile( p_chunk_x, p_chunk_y, p_chunk_z );
        if( l_tile && l_tile->ID == EMPTY_BLOCK_ID)
            break;
        return l_tile;
    }
    return NULL;
}

Chunk* world::getChunkWithPos( int x, int y, int z) {
    Chunk *node = Chunks;
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

void world::process_thrend() {
    for( int i = 0; i < (int)p_creatingList.size(); i++)
    {
        Chunk *l_node = getChunk( p_creatingList[i].x, p_creatingList[i].y, p_creatingList[i].z);
        if( l_node == NULL) {
            l_node = createChunk( p_creatingList[i].x, p_creatingList[i].y, p_creatingList[i].z);
            p_creatingList.erase( p_creatingList.begin()+ i);
            break;
        }
    }

    for( auto l_pos:p_deletingList)
    {
        Chunk *l_node = getChunk( l_pos.x, l_pos.y, l_pos.z);
        if( l_node != NULL)
            deleteChunk( l_node);
    }
    p_deletingList.clear();

    // be änderung Updaten
    updateArray();
}

void world::process( btDiscreteDynamicsWorld *world) {
    // Reset Idle time -> bis der Chunk sich selbst löscht
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        node->makeBulletMesh( world);
        node = node->next;
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
    // löschen des chunks
    destoryChunk( node->getPos().x, node->getPos().y, node->getPos().z);
}

Chunk *world::createChunk( int pos_x, int pos_y, int pos_z) {
    Chunk *node;

    // Chunk erstellen
    Timer timer;
    timer.Start();
    node = new Chunk( pos_x, pos_y, pos_z, 102457, p_blocklist);

    Landscape_Generator( node, p_blocklist);

    node->updateArray( p_blocklist);

    // Landscape erstellen
    //if( !p_clear)
    //    Landscape_Generator( node, p_blocklist);

    // seiten finden
    Chunk *snode;
    if( CheckChunk( pos_x+1, pos_y, pos_z)) {
        snode = getChunk( pos_x+1, pos_y, pos_z);
        snode->back = node;
        node->front = snode;
    }
    if( CheckChunk( pos_x-1, pos_y, pos_z)) {
        snode = getChunk( pos_x-1, pos_y, pos_z);
        snode->front = node;
        node->back = snode;
    }
    if( CheckChunk( pos_x, pos_y+1, pos_z)) {
        snode = getChunk( pos_x, pos_y+1, pos_z);
        snode->down = node;
        node->up = snode;
    }
    if( CheckChunk( pos_x, pos_y-1, pos_z)) {
        snode = getChunk( pos_x, pos_y-1, pos_z);
        snode->up = node;
        node->down = snode;
    }
    if( CheckChunk( pos_x, pos_y, pos_z+1)) {
        snode = getChunk( pos_x, pos_y, pos_z+1);
        snode->left = node;
        node->right = snode;
    }
    if( CheckChunk( pos_x, pos_y, pos_z-1)) {
        snode = getChunk( pos_x, pos_y, pos_z-1);
        snode->right = node;
        node->left = snode;
    }

    p_chunk_amount++; // Chunks mitzählen

    Chunk *tmp = Chunks;
    int z = 0;
    // Falls hauptzweik nicht exestiert erstllen
    if( Chunks == NULL) {
        Chunks = node;
    } else {
        while( tmp->next != NULL ) {
            tmp = tmp->next;
            z++;
        }
        tmp->next = node;
    }
    return node;
}

void world::destoryChunk( int pos_x, int pos_y, int pos_z) {
    Timer timer;
    timer.Start();
    Chunk *tmp = Chunks;
    Chunk *tmpOld = NULL;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->getPos().x == pos_x &&
             tmp->getPos().y == pos_y &&
             tmp->getPos().z == pos_z ) {
            if( tmpOld == NULL)
                Chunks = tmp->next;
            else tmpOld->next = tmp->next;

            // chunk seiten löschen
            Chunk *snode;
            if( CheckChunk( pos_x+1, pos_y, pos_z)) {
                snode = getChunk( pos_x+1, pos_y, pos_z);
                snode->back = NULL;
            }
            if( CheckChunk( pos_x-1, pos_y, pos_z)) {
                snode = getChunk( pos_x-1, pos_y, pos_z);
                snode->front = NULL;
            }
            if( CheckChunk( pos_x, pos_y+1, pos_z)) {
                snode = getChunk( pos_x, pos_y+1, pos_z);
                snode->down = NULL;
            }
            if( CheckChunk( pos_x, pos_y-1, pos_z)) {
                snode = getChunk( pos_x, pos_y-1, pos_z);
                snode->up = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z+1)) {
                snode = getChunk( pos_x, pos_y, pos_z+1);
                snode->left = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z-1)) {
                snode = getChunk( pos_x, pos_y, pos_z-1);
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

bool world::CheckChunk( int X, int Y, int Z) {
    Chunk *tmp = Chunks;
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



Chunk* world::getChunk( int X, int Y, int Z) {
    Chunk *tmp = Chunks;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->getPos().x == X &&
             tmp->getPos().y == Y &&
             tmp->getPos().z == Z )
             return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void world::addChunk( glm::tvec3<int> pos ) {
    p_creatingList.push_back( pos);
}
void world::addDeleteChunk( glm::tvec3<int> pos ) {
    p_deletingList.push_back( pos);
}

void world::draw( graphic *graphic, config *config, glm::mat4 viewProjection) {
    int g_width = graphic->getWidth();
    int g_height = graphic->getHeight();

    // get shader
    Shader* l_shader = graphic->getVoxelShader();

    // einstellung des shaders
    l_shader->Bind();
    l_shader->SetSize( (graphic->getDisplay()->getTilesetWidth()/16), ( graphic->getDisplay()->getTilesetHeight()/16) );

    p_image->Bind();

    // Transparent zeichnen
    drawTransparency( l_shader, viewProjection, true);
    drawTransparency( l_shader, viewProjection, false);

    // disable all Vertex arrays
    l_shader->disableArray( 0);
    l_shader->disableArray( 1);
    glUseProgram( 0 );
}

void world::drawTransparency( Shader* shader, glm::mat4 viewProjection, bool alpha_cutoff, glm::mat4 aa) {
    // shader einstellen
    if( alpha_cutoff) {
        glDepthMask(true);
        shader->SetAlpha_cutoff( 0.99f);
    } else {
        glDepthMask(false);
        shader->SetAlpha_cutoff( 1.10f);
    }
    // Draw node
    drawNode( shader,viewProjection, aa);
    glDepthMask(true);
}

void world::drawNode( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa) {
    Chunk *l_node = Chunks;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_node->draw( shader, viewProjection, aa);

        // next
        l_node = l_node->next;
    }
}

void world::updateArray() {
    Chunk *l_node = Chunks;
    for( ;; ) {
        if( l_node == NULL)
            break;
        l_node->updateArray( p_blocklist);

        l_node = l_node->next;
    }
}
