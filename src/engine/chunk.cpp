#include "chunk.h"
#include "../system/timer.h"
#include <stdio.h>

#define printf if

Chunk::Chunk( int X, int Y, int Z, int Seed, block_list* b_list) {
    // node reset
    next = NULL;

    // side reset
    front = NULL;
    back = NULL;
    right = NULL;
    left = NULL;
    up = NULL;
    down = NULL;


    p_rigidBody = NULL;

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

    for (int cz = 0; cz < CHUNK_SIZE; cz++)
        for (int cx = 0; cx < CHUNK_SIZE; cx++)
            for(int cy = 0; cy < CHUNK_SIZE; cy++) {
                int index = TILE_REGISTER( cx, cy, cz);
                p_tile[ index ] = EMPTY_BLOCK_ID;
            }

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

btRigidBody *Chunk::makeBulletMesh( btDiscreteDynamicsWorld *world) {
    btRigidBody *body = nullptr;

    if( !p_updateRigidBody)
        return body;

    // Handy lambda for converting from irr::vector to btVector
    auto toBtVector = [ &]( const glm::vec3 & vec ) -> btVector3
    {
        btVector3 bt( vec.x*CHUNK_SCALE, vec.y*CHUNK_SCALE, vec.z*CHUNK_SCALE );

        return bt;
    };

    // Make bullet rigid body
    if ( ! p_vertices.empty() && ! p_indices.empty() )
    {
        // Working numbers
        const size_t numIndices     = p_indices.size();
        const size_t numTriangles   = numIndices / 3;

        // Create triangles
        btTriangleMesh * btmesh = new btTriangleMesh();

        // Build btTriangleMesh
        for ( size_t i=0; i<numIndices; i+=3 )
        {
            const btVector3 &A = toBtVector( p_vertices[ p_indices[ i+0 ] ] );
            const btVector3 &B = toBtVector( p_vertices[ p_indices[ i+1 ] ] );
            const btVector3 &C = toBtVector( p_vertices[ p_indices[ i+2 ] ] );

            bool removeDuplicateVertices = true;
            btmesh->addTriangle( A, B, C, removeDuplicateVertices );
        }

        // Give it a default MotionState
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin( btVector3 ( p_form.getPos().x, p_form.getPos().y, p_form.getPos().z) );
        transform.setRotation( btQuaternion(0, 0, 0, 1) );

        btCollisionShape *btShape = new btBvhTriangleMeshShape( btmesh, true );

        // Give it a default MotionState
        btDefaultMotionState* fallMotionState =
                new btDefaultMotionState( transform);

        btScalar mass = 0;
        btVector3 fallInertia(0, 0, 0);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, btShape, fallInertia);
        body = new btRigidBody(fallRigidBodyCI);
    }

    if( p_rigidBody) {
        world->removeCollisionObject( p_rigidBody);
        delete p_rigidBody;
    }

    p_updateRigidBody = false;

    p_rigidBody = body;

    world->addRigidBody( p_rigidBody );
    return body;
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

void Chunk::updateForm()
{
    p_form.setPos( glm::vec3( p_pos.x*CHUNK_SIZE*CHUNK_SCALE, p_pos.y*CHUNK_SIZE*CHUNK_SCALE, p_pos.z*CHUNK_SIZE*CHUNK_SCALE) );
    p_form.setScale( glm::vec3( CHUNK_SCALE));
}

void Chunk::addFaceX( bool flip, glm::vec3 pos, glm::vec3 data) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_data.resize( p_data.size() + 4);

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

    // set data
    p_data[ l_vertices + 0] = data;
    p_data[ l_vertices + 1] = data;
    p_data[ l_vertices + 2] = data;
    p_data[ l_vertices + 3] = data;
}

void Chunk::addFaceY( bool flip, glm::vec3 pos, glm::vec3 data) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_data.resize( p_data.size() + 4);

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

    // set data
    p_data[ l_vertices + 0] = data;
    p_data[ l_vertices + 1] = data;
    p_data[ l_vertices + 2] = data;
    p_data[ l_vertices + 3] = data;
}

void Chunk::addFaceZ( bool flip, glm::vec3 pos, glm::vec3 data) {
    int l_flip = 0;
    int l_indices = p_indices.size();
    int l_vertices = p_vertices.size();

    p_indices.resize( p_indices.size() + 6);
    p_vertices.resize( p_vertices.size() + 4);
    p_data.resize( p_data.size() + 4);

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

    // set data
    p_data[ l_vertices + 0] = data;
    p_data[ l_vertices + 1] = data;
    p_data[ l_vertices + 2] = data;
    p_data[ l_vertices + 3] = data;
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
    p_data.clear( );

    bool b_visibility = false;

    int l_tile = 0;

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
                    Side_Textur_Pos = List->GetTexturByType( type, 0);
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
                    Side_Textur_Pos = List->GetTexturByType( type, 1);
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
                    p_vertices[ p_vertices.size() - 3] = glm::vec3( x, y, z);
                    p_vertices[ p_vertices.size() - 1] = glm::vec3( x, y, z+1);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 5);
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
                    Side_Textur_Pos = List->GetTexturByType( type, 4);
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
                    Side_Textur_Pos = List->GetTexturByType( type, 2);
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
                    Side_Textur_Pos = List->GetTexturByType( type, 2);
                    addFaceZ( true, glm::vec3( x, y, z+1), glm::vec3( Side_Textur_Pos.x, Side_Textur_Pos.y, 0));
                }
                b_visibility = true;
            }

        }
    }

    // normal errechnen
    int v;
    p_normal.resize( p_vertices.size());
    for( v = 0; v+3 <= i; v+=3) {
        glm::vec3 a(p_vertices[v].x, p_vertices[v].y, p_vertices[v].z);
        glm::vec3 b(p_vertices[v+1].x, p_vertices[v+1].y, p_vertices[v+1].z);
        glm::vec3 c(p_vertices[v+2].x, p_vertices[v+2].y, p_vertices[v+2].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        /*p_vertices[v+0].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));
        p_vertices[v+1].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));
        p_vertices[v+2].w = PackToFloat ( ConvertChar( normal.x) , ConvertChar( normal.y), ConvertChar( normal.z));*/

        p_normal[v] = normal;
        p_normal[v] = normal;
        p_normal[v] = normal;
        p_normal[v+1] = normal;
        p_normal[v+1] = normal;
        p_normal[v+1] = normal;
        p_normal[v+2] = normal;
        p_normal[v+2] = normal;
        p_normal[v+2] = normal;
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

void Chunk::updateVbo( Shader *shader) {
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
    glBufferData(GL_ARRAY_BUFFER, p_data.size() * sizeof(glm::vec3), &p_data[0], GL_STATIC_DRAW);

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
    glEnableVertexAttribArray( shader->getAntribute( 3));
    glBindBuffer( GL_ARRAY_BUFFER, p_vboData);
    glVertexAttribPointer( shader->getAntribute( 3), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    p_updateVbo = false;

    // print
    printf( "UpdateVbo %dms %d * %d = %d\n", timer.GetTicks(), sizeof(glm::vec3), getAmount(), getAmount() * sizeof(glm::vec3));
}

void Chunk::draw( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa) {
    if( p_vboVertex == 0 && !p_updateVbo)
        return;
    if( p_updateVbo)
        updateVbo( shader);

    // Shader einstellen
    shader->update( p_form.getModel(), viewProjection, aa);

    // use the vao
    glBindVertexArray( p_vboVao);

    // draw the elements
    glDrawElements( GL_TRIANGLES, p_elements, GL_UNSIGNED_INT, 0);

    // disable
    glBindVertexArray( 0);
}
