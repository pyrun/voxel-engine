#include "chunk.h"
#include "../system/timer.h"
#include <stdio.h>

#define printf if

Chunk::Chunk( int X, int Y, int Z, int Seed) {
    // node reset
    next = NULL;
    front = NULL;
    back = NULL;
    right = NULL;
    left = NULL;
    up = NULL;
    down = NULL;
    p_body = NULL;

    p_vboVertex = 0;
    p_elements = 0;

    p_changed = false;
    p_updateVbo = false;
    p_updateRigidBody = false;

    // Set Position
    p_pos.x = X;
    p_pos.y = Y;
    p_pos.z = Z;

    int t = SDL_GetTicks();

    p_tile = new int[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE]; //new (std::nothrow) tile*[ CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE]

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; i++)
        p_tile[ i ] = EMPTY_BLOCK_ID;

    updateForm();

    printf( "Chunk::chunk take %dms creating x%d y%d z%d\n", SDL_GetTicks()-t, p_pos.x, p_pos.y, p_pos.z);
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
    // Löschen p_tile
    for (int cz = 0; cz < CHUNK_SIZE; cz++)
        for (int cx = 0; cx < CHUNK_SIZE; cx++)
            for(int cy = 0; cy < CHUNK_SIZE; cy++) {
                    int Register = TILE_REGISTER( cx, cy, cz);
//                    delete p_tile[ Register];
//                    p_tile[ Register] = NULL;
                }
    delete[] p_tile;

    //printf( "~Chunk(): remove p_tile in %dms\n", timer.GetTicks());
    timer.Start();

    // vbo daten löschen
    glDeleteBuffers(1, &p_vboIndex);
    glDeleteBuffers(1, &p_vboVertex);
    glDeleteBuffers(1, &p_vboNormal);
    glDeleteBuffers(1, &p_vboData);
    glDeleteVertexArrays( 1, &p_vboVao);

    printf( "~Chunk(): remove vbo data in %dms x%dy%dz%d\n", timer.GetTicks(), p_pos.x, p_pos.y, p_pos.z);
}

bool Chunk::createPhysicBody( b3World *world) {
    if( !p_updateRigidBody && world)
        return false;

    if( p_body) {
        world->DestroyBody( p_body);
        p_body = NULL;
        delete p_shape;
        delete p_mesh;
        return false;
    }

    b3BodyDef l_bodyDef;

    l_bodyDef.type = b3BodyType::e_staticBody;
    l_bodyDef.position.Set( p_pos.x*CHUNK_SIZE*CHUNK_SCALE, p_pos.y*CHUNK_SIZE*CHUNK_SCALE, p_pos.z*CHUNK_SIZE*CHUNK_SCALE);

    p_body = world->CreateBody( l_bodyDef);

    //BuildGrid( &p_mesh, 50, 50);
    p_mesh = new b3Mesh();

    p_mesh->vertexCount = (int)p_vertices.size();
    p_mesh->vertices = (b3Vec3*)b3Alloc( p_mesh->vertexCount * sizeof(b3Vec3));

    for( int i = 0; i < (int)p_vertices.size(); i++) {
        p_mesh->vertices[i] = b3Vec3( p_vertices[i].x, p_vertices[i].y, p_vertices[i].z);
    }

    int l_triangleCount = (int)p_indices.size()/3;
    p_mesh->triangleCount  = l_triangleCount;
    p_mesh->triangles  = (b3Triangle*)b3Alloc(p_mesh->triangleCount * sizeof(b3Triangle));

    int l_triangle = 0;
    for( int i = 0; i < (int)p_indices.size(); i+=3) {
        p_mesh->triangles[ l_triangle].v1 = (int)p_indices[i+0];
        p_mesh->triangles[ l_triangle].v2 = (int)p_indices[i+1];
        p_mesh->triangles[ l_triangle].v3 = (int)p_indices[i+2];
        l_triangle++;
    }

    p_mesh->BuildTree();

    b3MeshShape l_meshShape;
    l_meshShape.m_mesh = p_mesh;


    p_shape = new b3ShapeDef();
    p_shape->shape = &l_meshShape;

    p_body->CreateShape( *p_shape);

    p_updateRigidBody = false;

    return true;
}

bool Chunk::serialize(bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end, block_list *blocks)
{
    for( int i = start; i < end; i++) {
        bitstream->Serialize( writeToBitstream, p_tile[i]);
        if( p_tile[i] > MAX_TILE_ID) {
            printf( "chunk::serialize corrupt data x%d y%d z%d Data %d to %d tileID#%d\n", (int)p_pos.x, (int)p_pos.y, (int)p_pos.z, start, end, p_tile[i]);
            p_tile[i] = EMPTY_BLOCK_ID;
            return false;
        }
        if(!writeToBitstream) {
            if( blocks->get( p_tile[i]) == NULL && p_tile[i] != EMPTY_BLOCK_ID ) {
                p_tile[i] = 0;
                return false;
            }
        }
    }
    return true;
}

void Chunk::set( int X, int Y, int Z, int ID, bool change) {
    if( X < 0)
        return;
    if( Y < 0)
        return;
    if( Z < 0)
        return;
    // tile nehmen
    p_tile[ TILE_REGISTER( X, Y, Z)] = ID;

    // welt hat sich verändert
    p_changed = change;
}

int Chunk::getTile( int X, int Y, int Z) {
    if( X < 0)
        return EMPTY_BLOCK_ID;
    if( Y < 0)
        return EMPTY_BLOCK_ID;
    if( Z < 0)
        return EMPTY_BLOCK_ID;
    return p_tile[ TILE_REGISTER( X, Y, Z)];
}

bool Chunk::CheckTile( int X, int Y, int Z) {
    if( p_tile[ TILE_REGISTER( X, Y, Z)] == EMPTY_BLOCK_ID)
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

void Chunk::addFaceX( bool flip, glm::vec3 pos, glm::vec3 texture) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_texture.resize( p_texture.size() + 4);

    // flip
    if( flip)
        l_flip = 5;

    // set indices
    p_indices[ l_indices + abs(0-l_flip) ] = l_vertices+0;
    p_indices[ l_indices + abs(1-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(2-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(3-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(4-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(5-l_flip) ] = l_vertices+3;

    // set vertices
    p_vertices[ l_vertices + 0] = pos + glm::vec3( 1, 0, 0);
    p_vertices[ l_vertices + 1] = pos + glm::vec3( 1, 1, 0);
    p_vertices[ l_vertices + 2] = pos + glm::vec3( 1, 0, 1);
    p_vertices[ l_vertices + 3] = pos + glm::vec3( 1, 1, 1);

    // set texture
    p_texture[ l_vertices + 0] = texture;
    p_texture[ l_vertices + 1] = texture;
    p_texture[ l_vertices + 2] = texture;
    p_texture[ l_vertices + 3] = texture;
}

void Chunk::addFaceY( bool flip, glm::vec3 pos, glm::vec3 texture) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_texture.resize( p_texture.size() + 4);

    // flip
    if( flip)
        l_flip = 5;

    // set indices
    p_indices[ l_indices + abs(0-l_flip) ] = l_vertices+0;
    p_indices[ l_indices + abs(1-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(2-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(3-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(4-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(5-l_flip) ] = l_vertices+3;

    // set vertices
    p_vertices[ l_vertices + 0] = pos + glm::vec3( 0, 0, 0);
    p_vertices[ l_vertices + 1] = pos + glm::vec3( 1, 0, 0);
    p_vertices[ l_vertices + 2] = pos + glm::vec3( 0, 0, 1);
    p_vertices[ l_vertices + 3] = pos + glm::vec3( 1, 0, 1);

    // set texture
    p_texture[ l_vertices + 0] = texture;
    p_texture[ l_vertices + 1] = texture;
    p_texture[ l_vertices + 2] = texture;
    p_texture[ l_vertices + 3] = texture;
}

void Chunk::addFaceZ( bool flip, glm::vec3 pos, glm::vec3 texture) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_texture.resize( p_texture.size() + 4);

    // flip
    if( flip)
        l_flip = 5;

    // set indices
    p_indices[ l_indices + abs(0-l_flip) ] = l_vertices+0;
    p_indices[ l_indices + abs(1-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(2-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(3-l_flip) ] = l_vertices+2;
    p_indices[ l_indices + abs(4-l_flip) ] = l_vertices+1;
    p_indices[ l_indices + abs(5-l_flip) ] = l_vertices+3;

    // set vertices
    p_vertices[ l_vertices + 0] = pos + glm::vec3( 0, 0, 0);
    p_vertices[ l_vertices + 1] = pos + glm::vec3( 0, 1, 0);
    p_vertices[ l_vertices + 2] = pos + glm::vec3( 1, 0, 0);
    p_vertices[ l_vertices + 3] = pos + glm::vec3( 1, 1, 0);

    // set texture
    p_texture[ l_vertices + 0] = texture;
    p_texture[ l_vertices + 1] = texture;
    p_texture[ l_vertices + 2] = texture;
    p_texture[ l_vertices + 3] = texture;
}

void Chunk::updateArray( block_list *List, Chunk *Back, Chunk *Front, Chunk *Left, Chunk *Right, Chunk *Up, Chunk *Down) {
    int i = 0;
    glm::vec2 Side_Textur_Pos;
    Timer timer;

    if( !List )
        return;

    timer.Start();

    p_indices.clear( );
    p_vertices.clear( );
    p_texture.clear( );

    bool b_visibility = false;

    int l_tile = 0;

    glm::vec3 l_pos = getPos();

    // View from positive x
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = CHUNK_SIZE - 1; x >= 0; x--) {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                l_tile = getTile( x, y, z);
                if( l_tile == EMPTY_BLOCK_ID) {// tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                int type = l_tile;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                // Line of sight blocked?
                if( x != 0 && CheckTile(x-1, y, z) && List->get( type)->getAlpha() == List->get( getTile( x-1, y, z) )->getAlpha() )
                {
                    b_visibility = false;
                    continue;
                }
                // View from negative x
                if( x == 0 && Back != NULL && Back->CheckTile(CHUNK_SIZE-1, y, z) && Back->getTile( CHUNK_SIZE-1, y, z)) {
                    b_visibility = false;
                    continue;
                }
                if( b_visibility && y != 0 && CheckTile(x, y-1, z) && ( l_tile == getTile( x, y-1, z)) ) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x, y+1, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x, y+1, z+1);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_FRONT);
                    addFaceX( true, glm::vec3( x-1, y, z), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 0));
                }

                b_visibility = true;
            }
        }
    }

    // View from negative x
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = 0; x < CHUNK_SIZE; x++)  {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                l_tile = getTile( x, y, z);
                if( !l_tile)
                    continue;

                if( l_tile == EMPTY_BLOCK_ID) {// tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                int type = l_tile;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }

                if(  x != CHUNK_SIZE-1 && CheckTile(x+1, y, z) && List->get( type)->getAlpha() == List->get( getTile( x+1, y, z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive x
                if( x == CHUNK_SIZE-1 && Front != NULL && Front->CheckTile( 0, y, z) && Front->getTile( 0, y, z)) {
                    b_visibility = false;
                    continue;
                }
                if( b_visibility && y != 0 && CheckTile(x, y-1, z) && ( l_tile == getTile( x, y-1, z)) ) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x+1, y+1, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x+1, y+1, z+1);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_BACK);
                    addFaceX( false, glm::vec3( x, y, z), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 0));
                }
                b_visibility = true;
            }
        }
    }

    // View from negative y
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int y = CHUNK_SIZE - 1; y >= 0; y--)
            for(int x = 0; x < CHUNK_SIZE; x++) {
              {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( y != 0 && CheckTile(x, y-1, z) && List->get( type)->getAlpha() == List->get( getTile( x, y-1, z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == 0 && Down != NULL && Down->CheckTile(x, CHUNK_SIZE-1, z) && Down->getTile( x, CHUNK_SIZE-1, z)) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z) == getTile( x, y, z)) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x+1, y, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x+1, y, z+1);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_BUTTOM);
                    addFaceY( false, glm::vec3( x, y, z), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 1));
                }
                b_visibility = true;
            }
        }
    }


    // View from positive y
    for(int z = 0; z < CHUNK_SIZE; z++) {
         for(int y = 0; y < CHUNK_SIZE; y++){
             for(int x = 0; x < CHUNK_SIZE; x++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  y != CHUNK_SIZE-1 && CheckTile(x, y+1, z) && List->get( type)->getAlpha() == List->get( getTile( x, y+1, z))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == CHUNK_SIZE-1 && Up != NULL && Up->CheckTile(x, 0, z) && Up->getTile( x, 0, z)) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z) == getTile( x, y, z)) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x+1, y+1, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x+1, y+1, z+1);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_TOP);
                    addFaceY( true, glm::vec3( x, y+1, z), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 1));
                }
                b_visibility = true;
            }
        }
    }

    // View from negative z
    for(int z = CHUNK_SIZE - 1; z >= 0; z--) {
         for(int x = 0; x < CHUNK_SIZE; x++){
            for(int y = 0; y < CHUNK_SIZE; y++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  z != 0 && CheckTile(x, y, z-1) && List->get( type)->getAlpha() == List->get( getTile( x, y, z-1))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( z == 0 && Left != NULL && Left->CheckTile(x, y, CHUNK_SIZE-1) && Left->getTile( x, y, CHUNK_SIZE-1)) {
                    b_visibility = false;
                    continue;
                }
                if( b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y, z) == getTile( x, y-1, z)) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x, y+1, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x+1, y+1, z);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_LEFT);
                    addFaceZ( false, glm::vec3( x, y, z), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 0));
                }
                b_visibility = true;
            }
        }
    }
    // View from positive z
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = 0; x < CHUNK_SIZE; x++) {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                int type = getTile( x, y, z);
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( z != CHUNK_SIZE-1 && CheckTile(x, y, z+1) && List->get( type)->getAlpha() == List->get( getTile( x, y, z+1))->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive z
                if( z == CHUNK_SIZE-1 && Right != NULL && Right->CheckTile(x, y, 0) && Right->getTile( x, y, 0)) {
                    b_visibility = false;
                    continue;
                }
                if( b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y-1, z) == getTile( x, y, z)) {
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x, y+1, z+1);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x+1, y+1, z+1);
                } else {
                    Side_Textur_Pos = List->getTexturByType( type, BLOCK_SIDE_RIGHT);
                    addFaceZ( true, glm::vec3( x, y, z+1), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 0));
                }
                b_visibility = true;
            }

        }
    }

    // normal errechnen
    p_normal.resize( p_vertices.size());
    for( int i = 0; i < (int)p_indices.size(); i+=3) {
        glm::vec3 a(p_vertices[ p_indices[i+0]].x, p_vertices[p_indices[i+0]].y, p_vertices[p_indices[i+0]].z);
        glm::vec3 b(p_vertices[ p_indices[i+1]].x, p_vertices[p_indices[i+1]].y, p_vertices[p_indices[i+1]].z);
        glm::vec3 c(p_vertices[ p_indices[i+2]].x, p_vertices[p_indices[i+2]].y, p_vertices[p_indices[i+2]].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        p_normal[p_indices[i+0]] = normal;
        p_normal[p_indices[i+1]] = normal;
        p_normal[p_indices[i+2]] = normal;
    }
    p_elements = p_indices.size();

    p_changed = false;

    if( p_elements == 0) {// Kein Speicher resavieren weil leer
        return;
    }

    p_updateVbo = true;
    p_updateRigidBody = true;

    printf( "Chunk::updateArray %dms %d %d %d\n", timer.GetTicks(), p_elements, getAmount());
}

void Chunk::updateForm()
{
    p_form.setPos( glm::vec3( p_pos.x*CHUNK_SIZE*CHUNK_SCALE, p_pos.y*CHUNK_SIZE*CHUNK_SCALE, p_pos.z*CHUNK_SIZE*CHUNK_SCALE) );
    p_form.setScale( glm::vec3( CHUNK_SCALE));
}

void Chunk::updateVbo() {
    Timer timer;
    timer.Start();

    // Nicht bearbeiten falls es anderweilig bearbeitet wird
    if( p_elements == 0)
        return;

    if( getVbo() == 0) {
        // create vao
        glGenVertexArrays(1, &p_vboVao);
        // create index buffer
        glGenBuffers(1, &p_vboIndex);

        // Create vbo
        glGenBuffers(1, &p_vboVertex);
        glGenBuffers(1, &p_vboNormal);
        glGenBuffers(1, &p_vboData);
    }

    // index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof( unsigned int ), &p_indices[0], GL_STATIC_DRAW);

    // vbo updaten
    // vertex
    glBindBuffer(GL_ARRAY_BUFFER, p_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, p_vertices.size() * sizeof(glm::vec3), &p_vertices[0], GL_STATIC_DRAW);
    // normal
    glBindBuffer(GL_ARRAY_BUFFER, p_vboNormal);
    glBufferData(GL_ARRAY_BUFFER, p_normal.size() * sizeof(glm::vec3), &p_normal[0], GL_STATIC_DRAW);
    // data
    glBindBuffer(GL_ARRAY_BUFFER, p_vboData);
    glBufferData(GL_ARRAY_BUFFER, p_texture.size() * sizeof(glm::vec3), &p_texture[0], GL_STATIC_DRAW);

    // VAO
    glBindVertexArray( p_vboVao);

    //array buffer bind
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vboIndex);

    // vertex
    glEnableVertexAttribArray(0);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboVertex);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboNormal);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // data
    glEnableVertexAttribArray( 2);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboData);
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    p_updateVbo = false;

    // print
    printf( "UpdateVbo %dms %d * %d = %d\n", timer.GetTicks(), sizeof(glm::vec3), getAmount(), getAmount() * sizeof(glm::vec3));
}

void Chunk::draw( Shader* shader, glm::mat4 view, glm::mat4 projection) {
    if( p_vboVertex == 0 && !p_updateVbo)
        return;
    if( p_updateVbo)
        updateVbo();

    // Shader einstellen
    shader->update( p_form.getModel(), view, projection);

    // use the vao
    glBindVertexArray( p_vboVao);

    // draw the elements
    glDrawElements( GL_TRIANGLES, p_elements, GL_UNSIGNED_INT, 0);

    // disable
    glBindVertexArray( 0);
}
