#include "world.h"
#include <time.h>
#include "../system/timer.h"

World::World( std::string Tileset, BlockList* B_List) {
    Seed = (int)time(NULL); // Seed
    p_buysvector = false;
    Tilemap = new Texture( Tileset);
    p_chunk_amount = 0;
    // Kein Chunk am anfang
    Chunks = NULL;
    p_world_tree_empty = true;
    // Blocklist link erstellen
    p_blocklist = B_List;
}

World::~World() {
    // Löschen der Welt
    DeleteChunks( Chunks);
    while( p_chunk_amount != 0) {
        SDL_Delay(1);
    }
    delete Tilemap;
}

Tile* World::GetTile( int x, int y, int z) {
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        int p_chunk_x = x-node->GetX()*CHUNK_WIDTH;
        int p_chunk_y = y-node->GetY()*CHUNK_HEIGHT;
        int p_chunk_z = z-node->GetZ()*CHUNK_DEPTH;

        // X - Achse
        if(p_chunk_x < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_x >= CHUNK_WIDTH ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if(p_chunk_y < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_y >= CHUNK_HEIGHT ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if(p_chunk_z < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_z >= CHUNK_DEPTH) {
            node = node->next;
            continue;
        }
        //printf("%d %d %d | ", p_chunk_x, p_chunk_y, p_chunk_z);
        Tile *l_tile = node->GetTile( p_chunk_x, p_chunk_y, p_chunk_z );
        if( l_tile && l_tile->ID == EMPTY_BLOCK_ID)
            break;
        return l_tile;
    }
    return NULL;
}

Chunk* World::GetChunkWithPos( int x, int y, int z) {
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        int p_chunk_x = x-node->GetX()*CHUNK_WIDTH;
        int p_chunk_y = y-node->GetY()*CHUNK_HEIGHT;
        int p_chunk_z = z-node->GetZ()*CHUNK_DEPTH;

        // X - Achse
        if(p_chunk_x < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_x >= CHUNK_WIDTH ) {
            node = node->next;
            continue;
        }
        // Y - Achse
        if(p_chunk_y < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_y >= CHUNK_HEIGHT ) {
            node = node->next;
            continue;
        }
        // Z - Achse
        if(p_chunk_z < 0 ) {
            node = node->next;
            continue;
        }
        if(p_chunk_z >= CHUNK_DEPTH) {
            node = node->next;
            continue;
        }
        return node;
    }
    return NULL;
}

void World::SetTile( Chunk *chunk, int tile_x, int tile_y, int tile_z, int ID) {
    if( chunk == NULL) {
        printf( "SetTile: Cant set tile!\n");
        return;
    }
    int chunk_x = chunk->GetX()*CHUNK_WIDTH;
    int chunk_y = chunk->GetY()*CHUNK_HEIGHT;
    int chunk_z = chunk->GetZ()*CHUNK_DEPTH;

    // Letzte prüfung
    //chunk->CreateTile( tile_x-chunk_x, tile_y-chunk_y, tile_z-chunk_z, ID);
    chunk->set( tile_x-chunk_x, tile_y-chunk_y, tile_z-chunk_z, ID);
}

void World::Process() {
    // be änderung Updaten
    UpdateArray();
    // chunks erstellen
    if( CheckChunk( 0, -1, 0) == false) {
        int factor = WORLD_TEST_FACTOR;
        for( int cx = -factor; cx <= factor; cx++)
            for( int cz = -factor; cz <= factor; cz++)
                for( int cy = 0; cy <= 0; cy++) {
            int tmp_x = cx;
            int tmp_y = cy;
            int tmp_z = cz;
            CreateChunk( tmp_x, tmp_y-1, tmp_z);
        }
        printf( "World::Process %d Chunks\n", GetAmountChunks());
    }
    // Reset Idle time -> bis der Chunk sich selbst löscht
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        node->ResetTimeIdle();
        node = node->next;
    }
}

void World::DeleteChunks( Chunk* chunk) {
    if( chunk == NULL )
        return;
    if( chunk->next )
        DeleteChunks( chunk->next );
    DeletingChunk( chunk->GetX(), chunk->GetY(), chunk->GetZ());
}

void World::AddChunk( int X, int Y, int Z) {
    World_Position Vector;
    Vector.x = X;
    Vector.y = Y;
    Vector.z = Z;
    if( p_buysvector == true)
        return;
    CreateChunkList.push_back( Vector);
}

void World::DeletingChunk( int pos_x, int pos_y, int pos_z) {
    World_Position Vector;
    Vector.x = pos_x;
    Vector.y = pos_y;
    Vector.z = pos_z;
    //while( p_buysvector == true);
    Chunk* node = GetChunk( pos_x, pos_y, pos_z);

    if( node == NULL)
        return;
    if( node->GetVbo() )
        node->DestoryVbo();
    // löschen des chunks
    DestoryChunk( pos_x, pos_y, pos_z);
    //DeletingChunkList.push_back( Vector);
}

void World::CreateChunk( int pos_x, int pos_y, int pos_z) {
    Chunk *node;

    // Chunk erstellen
    Timer timer;
    timer.Start();
    node = new Chunk( pos_x, pos_y, pos_z, 102457, p_blocklist);

    // Landscape erstellen

    Landscape_Generator( node, p_blocklist);
    //printf( "Create Chunk %dms %dkb\n", timer.GetTicks(), CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH*sizeof( Tile)/1024);

    // seiten finden
    Chunk *snode;
    if( CheckChunk( pos_x+1, pos_y, pos_z)) {
        snode = GetChunk( pos_x+1, pos_y, pos_z);
        snode->back = node;
        node->front = snode;
    }
    if( CheckChunk( pos_x-1, pos_y, pos_z)) {
        snode = GetChunk( pos_x-1, pos_y, pos_z);
        snode->front = node;
        node->back = snode;
    }
    if( CheckChunk( pos_x, pos_y+1, pos_z)) {
        snode = GetChunk( pos_x, pos_y+1, pos_z);
        snode->down = node;
        node->up = snode;
    }
    if( CheckChunk( pos_x, pos_y-1, pos_z)) {
        snode = GetChunk( pos_x, pos_y-1, pos_z);
        snode->up = node;
        node->down = snode;
    }
    if( CheckChunk( pos_x, pos_y, pos_z+1)) {
        snode = GetChunk( pos_x, pos_y, pos_z+1);
        snode->left = node;
        node->right = snode;
    }
    if( CheckChunk( pos_x, pos_y, pos_z-1)) {
        snode = GetChunk( pos_x, pos_y, pos_z-1);
        snode->right = node;
        node->left = snode;
    }
    //printf( "Add Chunk %d %d %d %d\n", p_chunk_amount+1, node.chunk->GetX(), node.chunk->GetY(), node.chunk->GetZ());
    //printf( "Add Chunk %d: %d %d %d\n", p_chunk_amount+1, node->GetX(), node->GetY(), node->GetZ());
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
    node->UpdateArray( p_blocklist, node->back, node->front, node->left, node->right, node->up, node->down);
    node->UpdateVbo();
}

void World::DestoryChunk( int pos_x, int pos_y, int pos_z) {
    Timer timer;
    timer.Start();
    Chunk *tmp = Chunks;
    Chunk *tmpOld = NULL;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->GetX() == pos_x &&
             tmp->GetY() == pos_y &&
             tmp->GetZ() == pos_z ) {
            if( tmpOld == NULL)
                Chunks = tmp->next;
            else tmpOld->next = tmp->next;

            // chunk seiten löschen
            Chunk *snode;
            if( CheckChunk( pos_x+1, pos_y, pos_z)) {
                snode = GetChunk( pos_x+1, pos_y, pos_z);
                snode->back = NULL;
            }
            if( CheckChunk( pos_x-1, pos_y, pos_z)) {
                snode = GetChunk( pos_x-1, pos_y, pos_z);
                snode->front = NULL;
            }
            if( CheckChunk( pos_x, pos_y+1, pos_z)) {
                snode = GetChunk( pos_x, pos_y+1, pos_z);
                snode->down = NULL;
            }
            if( CheckChunk( pos_x, pos_y-1, pos_z)) {
                snode = GetChunk( pos_x, pos_y-1, pos_z);
                snode->up = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z+1)) {
                snode = GetChunk( pos_x, pos_y, pos_z+1);
                snode->left = NULL;
            }
            if( CheckChunk( pos_x, pos_y, pos_z-1)) {
                snode = GetChunk( pos_x, pos_y, pos_z-1);
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

bool World::CheckChunk( int X, int Y, int Z) {
    Chunk *tmp = Chunks;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->GetX() == X &&
             tmp->GetY() == Y &&
             tmp->GetZ() == Z )
             return true;
        tmp = tmp->next;
    }
    return false;
}



Chunk* World::GetChunk( int X, int Y, int Z) {
    Chunk *tmp = Chunks;
    for( ;; ) {
        if( tmp == NULL)
            break;
         if( tmp->GetX() == X &&
             tmp->GetY() == Y &&
             tmp->GetZ() == Z )
             return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

float p = 0.0f;
bool b = true;

Camera *cam;

#define ota_size 200.0f

void World::Draw( Graphic *graphic, Config *config) {
    /*if( !Shadow.IsStarted()) {
        int w = graphic->GetWidth();
        int h = graphic->GetHeight();
        Shadow.Init( w, h );
        cam =  new Camera(glm::vec3( 0.0f, 0.0f, 0.0f), 1.0f, w/h, -850.0f, 20.0f, ota_size);
        //cam->GetPos() = graphic->GetCamera()->GetPos();
        cam->Pitch( +0.5);
        cam->RotateY( 0.1);
    }
    //cam->GetForward() = graphic->GetCamera()->GetForward();
    //cam->GetUp() = graphic->GetCamera()->GetUp();
    cam->GetPos() = graphic->GetCamera()->GetPos();
    cam->GetPos().x += ota_size/2;
    cam->GetPos().z += ota_size/2;
    cam->GetPos().x = (int)(cam->GetPos().x);
    cam->GetPos().y = (int)(cam->GetPos().y);
    cam->GetPos().z = (int)(cam->GetPos().z);

    if( 0) {
        Shadow.BindForWriting();
        //glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT);
        graphic->GetShadowShader()->Bind();// Shader
        if( p_world_tree != NULL && p_world_tree_empty == false)
            DrawNode(p_world_tree, graphic, graphic->GetShadowShader(), cam, cam );
        //glCullFace(GL_BACK);
    }
    if( 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        graphic->GetVoxelShader()->Bind();// Shader
        glActiveTexture(GL_TEXTURE0);
        Tilemap->Bind();
        graphic->GetVoxelShader()->SetTextureUnit( 1 );
        glActiveTexture( GL_TEXTURE1 );
        Shadow.BindForReading( GL_TEXTURE1);

        if( p_world_tree != NULL && p_world_tree_empty == false )
            DrawNode(p_world_tree, graphic, graphic->GetVoxelShader(), graphic->GetCamera(), cam);
    }*/

    /*if( 0) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        graphic->GetShadowShader()->Bind();// Shader
        //glActiveTexture(GL_TEXTURE0);
        graphic->GetShadowShader()->SetTextureUnit( 0);
        //Tilemap->Bind();
        //glActiveTexture(GL_TEXTURE1);
        Shadow.BindForReading(GL_TEXTURE0);
        if(p_world_tree != NULL && p_world_tree_empty == false)
            DrawNode(p_world_tree, graphic, graphic->GetShadowShader(), graphic->GetCamera());
    }*/

    int g_width = graphic->GetWidth();
    int g_height = graphic->GetHeight();

    //glClear(GL_ACCUp_BUFFER_BIT);

    // get shader
    Shader* l_shader = graphic->GetVoxelShader();

    // einstellung des shaders
    l_shader->Bind();
    l_shader->SetSize( (graphic->GetDisplay()->GetTilesetHeight()/16), ( graphic->GetDisplay()->GetTilesetWidth()/16) );

    // enable vertex array 0&1
    l_shader->EnableVertexArray( 0);
    l_shader->EnableVertexArray( 1);

    Tilemap->Bind();

    // How to not code a draw function... well played alex
    if( config->GetSupersampling()) {

        for( int alpha_cut = 0; alpha_cut < 1; alpha_cut++) {
            for(int i = 0; i < 4; i++) {
                glm::vec3 shift = glm::vec3((i % 4) * 0.5 / g_width, (i / 2) * 0.5 / g_height, 0);
                glm::mat4 aa = glm::translate(glm::mat4(1.0f), shift);

                // Zeichnen
                DrawTransparency(  graphic, l_shader, graphic->GetCamera(), graphic->GetCamera(), alpha_cut == 1 ? false : true, aa);
                glAccum(i ? GL_ACCUM : GL_LOAD, 0.25f);
            }
            // zusammenrechnen
            glAccum(GL_RETURN, 1);
        }
        // Zeichne Transperenz wenn gewünscht
        if( config->GetTransparency())
            DrawTransparency(  graphic, l_shader, graphic->GetCamera(), graphic->GetCamera(), false);
    } else {
        if( config->GetTransparency() ) {
            // Transparent zeichnen
            DrawTransparency( graphic, l_shader, graphic->GetCamera(), graphic->GetCamera(), true);
            DrawTransparency( graphic, l_shader, graphic->GetCamera(), graphic->GetCamera(), false);
        } else {
            // Ohne Transperenz -> Sehr schnell
            graphic->GetVoxelShader()->SetAlpha_cutoff( 0.0f);
            glDisable( GL_BLEND);
            DrawNode( graphic, l_shader, graphic->GetCamera(), graphic->GetCamera());
            glEnable( GL_BLEND);
        }
    }
}

void World::DrawTransparency( Graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, bool alpha_cutoff, glm::mat4 aa) {
    // shader einstellen
    if( alpha_cutoff) {
        glDepthMask(true);
        shader->SetAlpha_cutoff( 0.99f);
    } else {
        glDepthMask(false);
        shader->SetAlpha_cutoff( 1.10f);
    }
    // Draw node
    DrawNode( graphic, shader, camera, camera, aa);
    glDepthMask(true);
}

void World::DrawNode( Graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, glm::mat4 aa) {
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        // Update Chunk if Change
        if( node->GetArrayChange()) {
            node->UpdateVbo();
        }
        if( node->GetUpdateOnce() && node->GetAmount() != 0 && node->GetUpdateVboOnce()) {
            if( SDL_GetTicks() - node->GetTimeIdle() > WORLD_TILE_IDLE_TIME ) {
                DeletingChunk( node->GetX(), node->GetY(), node->GetZ() );
            } else {
                node->Draw( graphic, shader, camera, shadow, aa);
            }
        }
        node = node->next;
    }
}
void World::UpdateArray() {
    UpdateArrayNode();
}

void World::UpdateArrayNode() {
    Chunk *node = Chunks;
    for( ;; ) {
        if( node == NULL)
            break;
        // Update Chunk if Change
        if( (node->GetChanged() || !node->GetUpdateOnce() ) && node->IsDrawable()) {// Update Chunk -> es hat sich was verändert
            node->UpdateArray( p_blocklist, node->back, node->front, node->left, node->right, node->up, node->down);
        }
        node = node->next;
    }
}
