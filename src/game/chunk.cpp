#include "chunk.h"
#include "../system/timer.h"
#include <stdio.h>

ChunkVboDataStruct CVDS_SetBlock( GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    ChunkVboDataStruct New;
    New.x = x;
    New.y = y;
    New.z = z;
    New.w = w;
    return New;
}

ChunkVboVertexStruct CVDS_SetVertex( GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    ChunkVboVertexStruct New;
    New.x = x;
    New.y = y;
    New.z = z;
    New.w = w;
    return New;
}

Chunk::Chunk( int X, int Y, int Z, int Seed, BlockList* b_list) {
    // node reset
    next = NULL;
    // side reset
    front = NULL;
    back = NULL;
    right = NULL;
    left = NULL;
    up = NULL;
    down = NULL;
    m_time_idle = SDL_GetTicks();
    // falgs
    m_nomorevbo = false;
    m_updateonce = false;
    m_changed = false;
    m_elements = 0;
    m_createvbo = false;
    m_arraychange = false;
    m_updatevboonce = false;
    m_updatevbo = false;
    m_deleting = false;
    m_seed = Seed;
    // Set Position
    this->x = X;
    this->y = Y;
    this->z = Z;
    m_tile = NULL;



    m_tile = (Tile**)malloc(sizeof(Tile*) * CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH); //new (std::nothrow) Tile*[ CHUNK_WIDTH*CHUNK_HEIGHT*CHUNK_DEPTH];

    if (m_tile == nullptr) {
      // error assigning memory. Take measures.
      printf( "Chunk::Chunk error\n");
      return;
    }

    int t = SDL_GetTicks();
    for (int cz = 0; cz < CHUNK_DEPTH; cz++)
        for (int cx = 0; cx < CHUNK_WIDTH; cx++)
            for(int cy = 0; cy < CHUNK_HEIGHT; cy++) {
                int index = TILE_REGISTER( cx, cy, cz);
                m_tile[ index ] = NULL;
                //CreateTile( cx, cy, cz, 0);
            }
    printf( "Chunk::chunk %d\n", SDL_GetTicks()-t);
    // http://www.blitzbasic.com/Community/posts.php?topic=93982
}

Chunk::~Chunk() {
    Timer timer;
    timer.Start();
    // node reset
    next = NULL;
    // side reset
    front = NULL;
    back = NULL;
    right = NULL;
    left = NULL;
    up = NULL;
    down = NULL;
    // Löschen m_tile
    for (int cz = 0; cz < CHUNK_DEPTH; cz++)
        for (int cx = 0; cx < CHUNK_WIDTH; cx++)
            for(int cy = 0; cy < CHUNK_HEIGHT; cy++) {
                    int Register = TILE_REGISTER( cx, cy, cz);
                    delete m_tile[ Register];
                    m_tile[ Register] = NULL;
                }
    delete[] m_tile;

    //printf( "~Chunk(): remove m_tile in %dms\n", timer.GetTicks());
    timer.Start();
    // vbo daten löschen
    m_data.clear();
    m_vertex.clear();
    //m_normal.clear();
    //printf( "~Chunk(): remove vbo data in %dms\n", timer.GetTicks());
}

void Chunk::CreateTile( int X, int Y, int Z, int ID) {
    Tile* tile;
    tile = GetTile( X, Y, Z);
    if( ID == 0) {
        if( tile != NULL) {
            m_changed = true;
            delete m_tile[ TILE_REGISTER( X, Y, Z)];
            m_tile[ TILE_REGISTER( X, Y, Z)] = NULL;
        }
        return;
    }

    // erst checken das dieser Tile schon vorhanden ist sonst Crash
    if ( tile == NULL) {
        // erstellen von dem Tile
        tile = m_tile[ TILE_REGISTER( X, Y, Z)] = new Tile;
    }
    // tile nehmen
    tile->ID = ID;
    tile->m_left = TILE_VERTEX_NULL;
    tile->m_right = TILE_VERTEX_NULL;
    tile->m_up = TILE_VERTEX_NULL;
    tile->m_down = TILE_VERTEX_NULL;
    tile->m_forward = TILE_VERTEX_NULL;
    tile->m_back = TILE_VERTEX_NULL;
    // welt hat sich verändert
    m_changed = true;
}

Tile *Chunk::GetTile( int X, int Y, int Z) {
    if( X < 0)
        return NULL;
    if( Y < 0)
        return NULL;
    if( Z < 0)
        return NULL;
    // X Y Z
    // Z X Y -> https://www.youtube.com/watch?v=B4DuT61lIPU
    return m_tile[ TILE_REGISTER( X, Y, Z)];
}

bool Chunk::CheckTile( int X, int Y, int Z) {
    if( m_tile[ TILE_REGISTER( X, Y, Z)] == NULL)
        return false;
    return true;
}

//Helper method to go from a float to packed char
unsigned char ConvertChar(float value) {
  //Scale and bias
  value = (value + 1.0f) * 0.5f;
  return (unsigned char)(value*255.0f);
}

//Pack 3 values into 1 float
float PackToFloat(unsigned char x, unsigned char y, unsigned char z)
{
  unsigned int packedColor = (x << 16) | (y << 8) | z;
  float packedFloat = (float) ( ((double)packedColor) / ((double) (1 << 24)) );

  return packedFloat;
}

void Chunk::UpdateArray( BlockList *List, Chunk *Back, Chunk *Front, Chunk *Left, Chunk *Right, Chunk *Up, Chunk *Down) {
    int i = 0;
    glm::vec2 Side_Textur_Pos;
    Timer timer;
    timer.Start();
    // Leeren
    m_vertex.resize(0);
    m_data.resize(0);

    // wird gelöscht
    if( m_deleting)
        return;

    bool b_visibility = false;

    // View from negative x
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = CHUNK_WIDTH - 1; x >= 0; x--) {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( GetTile( x, y, z) == NULL) {// Tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                unsigned int type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                // Line of sight blocked?
                if(  x != 0 && CheckTile(x-1, y, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x-1, y, z)->ID )->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from negative x
                if( x == 0 && Back != NULL && Back->CheckTile(CHUNK_WIDTH-1, y, z) && Back->GetTile( CHUNK_WIDTH-1, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if( b_visibility && y != 0 && CheckTile(x, y-1, z) && (GetTile( x, y, z)->ID == GetTile( x, y-1, z)->ID) ) {
                    m_vertex[i - 4] = CVDS_SetVertex( x,     y+1,     z, 0);
                    m_vertex[i - 3] = CVDS_SetVertex( x, y+1, z, 0);
                    m_vertex[i - 1] = CVDS_SetVertex( x, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 0);
                    m_vertex.resize( m_vertex.size()+6 );
                    m_data.resize( m_data.size()+6 );

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from positive x
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = 0; x < CHUNK_WIDTH; x++)  {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( GetTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  x != CHUNK_WIDTH-1 && CheckTile(x+1, y, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x+1, y, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive x
                if( x == CHUNK_WIDTH-1 && Front != NULL && Front->CheckTile( 0, y, z) && Front->GetTile( 0, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile( x, y-1, z) && GetTile( x, y, z)->ID == GetTile( x, y-1, z)->ID) {
                    m_vertex[i - 5] = CVDS_SetVertex(x + 1, y+1, z, 0);
                    m_vertex[i - 3] = CVDS_SetVertex(x +1, y+1, z, 0);
                    m_vertex[i - 2] = CVDS_SetVertex(x +1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 1);
                    m_vertex.resize( m_vertex.size()+6);
                    m_data.resize( m_data.size()+6);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }

    // View from negative y
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int y = CHUNK_HEIGHT - 1; y >= 0; y--)
            for(int x = 0; x < CHUNK_WIDTH; x++) {
              {
                if( GetTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( y != 0 && CheckTile(x, y-1, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x, y-1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == 0 && Down != NULL && Down->CheckTile(x, CHUNK_HEIGHT-1, z) && Down->GetTile( x, CHUNK_HEIGHT-1, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && GetTile( x-1, y, z)->ID == GetTile( x, y, z)->ID) {
                    m_vertex[i - 5] = CVDS_SetVertex(x+1    , y, z, 0);
                    m_vertex[i - 3] = CVDS_SetVertex(x + 1, y, z, 0);
                    m_vertex[i - 2] = CVDS_SetVertex(x + 1, y, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 5);
                    m_vertex.resize( m_vertex.size()+6);
                    m_data.resize( m_data.size()+6);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from positive y
    for(int z = 0; z < CHUNK_DEPTH; z++) {
         for(int y = 0; y < CHUNK_HEIGHT; y++){
             for(int x = 0; x < CHUNK_WIDTH; x++) {
                if( GetTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  y != CHUNK_HEIGHT-1 && CheckTile(x, y+1, z) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x, y+1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == CHUNK_HEIGHT-1 && Up != NULL && Up->CheckTile(x, 0, z) && Up->GetTile( x, 0, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && GetTile( x-1, y, z)->ID == GetTile( x, y, z)->ID) {
                    m_vertex[i - 4] = CVDS_SetVertex( x+1,     y+1,     z, 0);
                    m_vertex[i - 3] = CVDS_SetVertex( x+1, y+1, z, 0);
                    m_vertex[i - 1] = CVDS_SetVertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 4);
                    m_vertex.resize( m_vertex.size()+6);
                    m_data.resize( m_data.size()+6);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 1, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from negative z
    for(int z = CHUNK_DEPTH - 1; z >= 0; z--) {
         for(int x = 0; x < CHUNK_WIDTH; x++){
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( GetTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  z != 0 && CheckTile(x, y, z-1) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x, y, z-1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( z == 0 && Left != NULL && Left->CheckTile(x, y, CHUNK_DEPTH-1) && Left->GetTile( x, y, CHUNK_DEPTH-1)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && GetTile( x, y, z)->ID == GetTile( x, y-1, z)->ID) {
                    m_vertex[i - 5] = CVDS_SetVertex(x, y+1, z, 0);
                    m_vertex[i - 2] = CVDS_SetVertex(x+1, y+1, z, 0);
                    m_vertex[i - 3] = CVDS_SetVertex(x, y+1, z, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 2);
                    m_vertex.resize( m_vertex.size()+6);
                    m_data.resize( m_data.size()+6);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z, 0);
                }
                b_visibility = true;
            }
        }
    }
    // View from positive z
    for(int z = 0; z < CHUNK_DEPTH; z++) {
        for(int x = 0; x < CHUNK_WIDTH; x++) {
            for(int y = 0; y < CHUNK_HEIGHT; y++) {
                if( GetTile( x, y, z) == NULL) // Tile nicht vorhanden
                    continue;
                uint8_t type = GetTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( z != CHUNK_DEPTH-1 && CheckTile(x, y, z+1) && List->GetBlock( type)->getAlpha() == List->GetBlock( GetTile( x, y, z+1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive z
                if( z == CHUNK_DEPTH-1 && Right != NULL && Right->CheckTile(x, y, 0) && Right->GetTile( x, y, 0)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && GetTile( x, y-1, z)->ID == GetTile( x, y, z)->ID) {
                    m_vertex[i - 4] = CVDS_SetVertex( x,     y+1,     z+1, 0);
                    m_vertex[i - 3] = CVDS_SetVertex( x, y+1, z+1, 0);
                    m_vertex[i - 1] = CVDS_SetVertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 3);
                    m_vertex.resize( m_vertex.size()+6);
                    m_data.resize( m_data.size()+6);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x, y + 1, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y, z + 1, 0);

                    m_data[i] = CVDS_SetBlock( 0, Side_Textur_Pos.x, Side_Textur_Pos.y);
                    m_vertex[i++] = CVDS_SetVertex(x + 1, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }

        }
    }
    // normal errechnen
    int v;
    //m_normal.resize( m_vertex.size());
    for( v = 0; v+3 <= i; v+=3) {
        glm::vec3 a(m_vertex[v].x, m_vertex[v].y, m_vertex[v].z);
        glm::vec3 b(m_vertex[v+1].x, m_vertex[v+1].y, m_vertex[v+1].z);
        glm::vec3 c(m_vertex[v+2].x, m_vertex[v+2].y, m_vertex[v+2].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        m_vertex[v+0].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));
        m_vertex[v+1].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));
        m_vertex[v+2].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));

        /*m_normal[v].x = normal.x;
        m_normal[v].y = normal.y;
        m_normal[v].z = normal.z;

        m_normal[v+1].x = normal.x;
        m_normal[v+1].y = normal.y;
        m_normal[v+1].z = normal.z;

        m_normal[v+2].x = normal.x;
        m_normal[v+2].y = normal.y;
        m_normal[v+2].z = normal.z;*/
    }
    m_elements = i;
    m_changed = false;
    m_updateonce = true;
    if( m_elements == 0) {// Kein Speicher resavieren weil leer
        return;
    }
    m_arraychange = true;
    //printf( "UpdateArray %dms %d %d %d\n", timer.GetTicks(), m_elements, m_vertex.size(), m_data.size());
}



void Chunk::DestoryVbo() {
    m_nomorevbo = true;
    while( m_updatevbo);

    if( m_createvbo ) {
        m_createvbo = false;
        glDeleteBuffers(1, &m_vboVertex);
//        glDeleteBuffers(1, &m_vboNormal);
        glDeleteBuffers(1, &m_vboData);
    }
}

void Chunk::UpdateVbo() {
    Timer timer;
    timer.Start();

    // Nicht bearbeiten falls es anderweilig bearbeitet wird
    if( m_deleting)
        return;
    if( m_elements == 0 || m_updateonce == false || m_nomorevbo == true)
        return;

    // VBO erstellen falls dieser fehlt
    if(m_createvbo == false) {
        // Create vbo
        glGenBuffers(1, &m_vboVertex);
//        glGenBuffers(1, &m_vboNormal);
        glGenBuffers(1, &m_vboData);
        m_createvbo = true;
    }

    // flags ändern
    m_arraychange = false;
    m_updatevboonce = true;

    // anderweilig beschäftigt?
    while( m_updatevbo);
    m_updatevbo = true;
    // vbo updaten
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(ChunkVboVertexStruct), &m_vertex[0], GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, m_vboNormal);
//    glBufferData(GL_ARRAY_BUFFER, m_normal.size() * sizeof(ChunkVboDataStruct), &m_normal[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboData);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(ChunkVboDataStruct), &m_data[0], GL_STATIC_DRAW);
    m_updatevbo = false;
    //printf( "UpdateVbo %dms %d * %d = %d\n", timer.GetTicks(), sizeof(ChunkVboDataStruct), m_vertex.size(), m_vertex.size() * sizeof(ChunkVboDataStruct));
}

void Chunk::Draw( Graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, glm::mat4 aa) {
    // Position errechnen
    Transform f_form;
    f_form.GetPos().x = x*CHUNK_WIDTH;
    f_form.GetPos().y = y*CHUNK_HEIGHT;
    f_form.GetPos().z = z*CHUNK_DEPTH;

    // chunk wird grad gelöscht
    if( m_nomorevbo == true)
        return;
    // wird gelöscht
    if( m_deleting)
        return;
    if( m_updatevbo)
        return;
    m_updatevbo = true;

    // Shader einstellen
    shader->Update( f_form, camera, shadow, aa);

    // vbo pointer auf array setzen
    shader->BindArray( m_vboVertex, 0, GL_FLOAT);
    shader->BindArray( m_vboData, 1);

    // Dreiecke zeichnen
    // Debug GL_LINES, GL_TRIANGLES
    glDrawArrays( GL_TRIANGLES, 0, (int)m_elements);

    // Update war erfolgreich
    m_updatevbo = false;
}
