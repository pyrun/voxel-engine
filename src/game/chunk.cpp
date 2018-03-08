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
    p_time_idle = SDL_GetTicks();
    p_rigidBody = NULL;

    // falgs
    p_nomorevbo = false;
    p_updateonce = false;
    p_changed = false;
    p_elements = 0;
    p_createvbo = false;
    p_arraychange = false;
    p_updatevboonce = false;
    p_updatevbo = false;
    p_deleting = false;
    p_physicSet = false;
    p_seed = Seed;

    // Set Position
    p_pos.x = X;
    p_pos.y = Y;
    p_pos.z = Z;

    p_tile = NULL;

    int t = SDL_GetTicks();

    p_tile = (tile *)malloc(sizeof(tile) * CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE); //new (std::nothrow) tile*[ CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE]

    if (p_tile == nullptr) {
      // error assigning memory. Take measures.
      printf( "Chunk::Chunk error\n");
      return;
    }

    for (int cz = 0; cz < CHUNK_SIZE; cz++)
        for (int cx = 0; cx < CHUNK_SIZE; cx++)
            for(int cy = 0; cy < CHUNK_SIZE; cy++) {
                int index = TILE_REGISTER( cx, cy, cz);
                p_tile[ index ].ID = EMPTY_BLOCK_ID;
            }

    updateForm();

    printf( "Chunk::chunk take %dms creating x%dy%dz%d\n", SDL_GetTicks()-t, p_pos.x, p_pos.y, p_pos.z);
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
/*    p_data.clear();
    p_vertices.clear();*/
    //p_normal.clear();
    printf( "~Chunk(): remove vbo data in %dms x%dy%dz%d\n", timer.GetTicks(), p_pos.x, p_pos.y, p_pos.z);
}

btRigidBody *Chunk::makeBulletMesh() {
    btRigidBody * body = nullptr;

    // Handy lambda for converting from irr::vector to btVector
    auto toBtVector = [ &]( const block_vertex & vec ) -> btVector3
    {
        btVector3 bt( vec.x*CHUNK_SCALE, vec.y*CHUNK_SCALE, vec.z*CHUNK_SCALE );

        return bt;
    };

    // Make bullet rigid body
    if ( ! p_vertices.empty() )
    {
        // Working numbers
        const size_t numIndices     = p_vertices.size();
        const size_t numTriangles   = numIndices / 3;

        // Create triangles
        btTriangleMesh * btmesh = new btTriangleMesh();

        // Build btTriangleMesh
        for ( size_t i=0; i<numIndices; i+=3 )
        {
            const btVector3 &A = toBtVector( p_vertices[ i+0 ] );
            const btVector3 &B = toBtVector( p_vertices[ i+1 ] );
            const btVector3 &C = toBtVector( p_vertices[ i+2 ] );

            bool removeDuplicateVertices = true;
            btmesh->addTriangle( A, B, C, removeDuplicateVertices );
        }


        // Give it a default MotionState
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin( btVector3 ( p_form.getPos().x, p_form.getPos().y, p_form.getPos().z) );
        transform.setRotation( btQuaternion(0, 0, 0, 1) );

        /*
        btDefaultMotionState *motionState = new btDefaultMotionState( transform );

        btCollisionShape *btShape = new btBvhTriangleMeshShape( btmesh, true );

        btRigidBody::btRigidBodyConstructionInfo
                groundRigidBodyCI(0, motionState, btShape, btVector3(0, 0, 0));
        body = new btRigidBody(groundRigidBodyCI);*/

                // Give it a default MotionState
        btDefaultMotionState* fallMotionState =
                new btDefaultMotionState( transform);

        btConvexShape  *tmpshape = new btConvexTriangleMeshShape( btmesh );
        btShapeHull *hull = new btShapeHull(tmpshape);
        btScalar margin = tmpshape->getMargin();
        hull->buildHull(margin);
        btConvexHullShape* simplifiedConvexShape = new btConvexHullShape( (btScalar*)hull->getVertexPointer(), hull->numVertices());

        btScalar mass = 0;
        btVector3 fallInertia(0, 0, 0);
        //btShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, tmpshape, fallInertia);
        body = new btRigidBody(fallRigidBodyCI);
    }
    return body;
}

void Chunk::CreateTile( int X, int Y, int Z, int ID) {
    tile* l_tile;
    l_tile = getTile( X, Y, Z);

    // cant happen but make sure
    if( l_tile == NULL)
        return;

    // tile nehmen
    l_tile->ID = ID;

    // welt hat sich verändert
    p_changed = true;
}

void Chunk::set( int X, int Y, int Z, int ID) {
    tile* l_tile;
    l_tile = getTile( X, Y, Z);
    if( l_tile == NULL)
        return;

    // tile nehmen
    l_tile->ID = ID;

    // welt hat sich verändert
    p_changed = true;
}

tile *Chunk::getTile( int X, int Y, int Z) {
    if( X < 0)
        return NULL;
    if( Y < 0)
        return NULL;
    if( Z < 0)
        return NULL;
    // X Y Z
    // Z X Y -> https://www.youtube.com/watch?v=B4DuT61lIPU
    tile *l_tile = &p_tile[ TILE_REGISTER( X, Y, Z)];
    return l_tile;
}

bool Chunk::CheckTile( int X, int Y, int Z) {
    if( p_tile[ TILE_REGISTER( X, Y, Z)].ID == EMPTY_BLOCK_ID)
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

void Chunk::UpdateArray( block_list *List, Chunk *Back, Chunk *Front, Chunk *Left, Chunk *Right, Chunk *Up, Chunk *Down) {
    int i = 0;
    glm::vec2 Side_Textur_Pos;
    Timer timer;
    timer.Start();

    p_vertices.reserve( 2048);
    p_data.reserve( 2048);

    // wird gelöscht
    if( p_deleting)
        return;

    bool b_visibility = false;

    tile * l_tile = NULL;

    // View from negative x
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = CHUNK_SIZE - 1; x >= 0; x--) {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                l_tile = getTile( x, y, z);
                if( !l_tile)
                    continue;

                if( l_tile->ID == EMPTY_BLOCK_ID) {// tile nicht vorhanden
                    b_visibility = false;
                    continue;
                }
                unsigned int type = l_tile->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                // Line of sight blocked?
                if(  x != 0 && CheckTile(x-1, y, z) && List->get( type)->getAlpha() == List->get( getTile( x-1, y, z)->ID )->getAlpha() )
                {
                    b_visibility = false;
                    continue;
                }
                // View from negative x
                if( x == 0 && Back != NULL && Back->CheckTile(CHUNK_SIZE-1, y, z) && Back->getTile( CHUNK_SIZE-1, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if( 0&& b_visibility && y != 0 && CheckTile(x, y-1, z) && ( l_tile->ID == getTile( x, y-1, z)->ID) ) {
                    p_vertices[i - 4] = block_vertex( x,     y+1,     z, 0);
                    p_vertices[i - 3] = block_vertex( x, y+1, z, 0);
                    p_vertices[i - 1] = block_vertex( x, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 0);

                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0.f, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0.f);
                    p_vertices[i++] = block_vertex(x, y + 1, z + 1, 0);
                }

                b_visibility = true;
            }
        }
    }
    // View from positive x
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int x = 0; x < CHUNK_SIZE; x++)  {
            for(int y = 0; y < CHUNK_SIZE; y++) {
                if( getTile( x, y, z) == NULL) // tile nicht vorhanden
                    continue;
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  x != CHUNK_SIZE-1 && CheckTile(x+1, y, z) && List->get( type)->getAlpha() == List->get( getTile( x+1, y, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive x
                if( x == CHUNK_SIZE-1 && Front != NULL && Front->CheckTile( 0, y, z) && Front->getTile( 0, y, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile( x, y-1, z) && getTile( x, y, z)->ID == getTile( x, y-1, z)->ID) {
                    p_vertices[i - 5] = block_vertex(x + 1, y+1, z, 0);
                    p_vertices[i - 3] = block_vertex(x +1, y+1, z, 0);
                    p_vertices[i - 2] = block_vertex(x +1, y+1, z+1, 0);
                } else {
                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    Side_Textur_Pos = List->GetTexturByType( type, 1);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z + 1, 0);
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
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( y != 0 && CheckTile(x, y-1, z) && List->get( type)->getAlpha() == List->get( getTile( x, y-1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == 0 && Down != NULL && Down->CheckTile(x, CHUNK_SIZE-1, z) && Down->getTile( x, CHUNK_SIZE-1, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z)->ID == getTile( x, y, z)->ID) {
                    p_vertices[i - 5] = block_vertex(x+1    , y, z, 0);
                    p_vertices[i - 3] = block_vertex(x + 1, y, z, 0);
                    p_vertices[i - 2] = block_vertex(x + 1, y, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 5);

                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y, z + 1, 0);
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
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  y != CHUNK_SIZE-1 && CheckTile(x, y+1, z) && List->get( type)->getAlpha() == List->get( getTile( x, y+1, z)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( y == CHUNK_SIZE-1 && Up != NULL && Up->CheckTile(x, 0, z) && Up->getTile( x, 0, z)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && x != 0 && CheckTile(x-1, y, z) && getTile( x-1, y, z)->ID == getTile( x, y, z)->ID) {
                    p_vertices[i - 4] = block_vertex( x+1,     y+1,     z, 0);
                    p_vertices[i - 3] = block_vertex( x+1, y+1, z, 0);
                    p_vertices[i - 1] = block_vertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 4);

                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 1, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z + 1, 0);
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
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if(  z != 0 && CheckTile(x, y, z-1) && List->get( type)->getAlpha() == List->get( getTile( x, y, z-1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                if( z == 0 && Left != NULL && Left->CheckTile(x, y, CHUNK_SIZE-1) && Left->getTile( x, y, CHUNK_SIZE-1)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y, z)->ID == getTile( x, y-1, z)->ID) {
                    p_vertices[i - 5] = block_vertex(x, y+1, z, 0);
                    p_vertices[i - 2] = block_vertex(x+1, y+1, z, 0);
                    p_vertices[i - 3] = block_vertex(x, y+1, z, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 2);

                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z, 0);
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
                uint8_t type = getTile( x, y, z)->ID;
                if( type == 0) {
                    b_visibility = false;
                    continue;
                }
                if( z != CHUNK_SIZE-1 && CheckTile(x, y, z+1) && List->get( type)->getAlpha() == List->get( getTile( x, y, z+1)->ID)->getAlpha() ) {
                    b_visibility = false;
                    continue;
                }
                // View from positive z
                if( z == CHUNK_SIZE-1 && Right != NULL && Right->CheckTile(x, y, 0) && Right->getTile( x, y, 0)->ID) {
                    b_visibility = false;
                    continue;
                }
                if(b_visibility && y != 0 && CheckTile(x, y-1, z) && getTile( x, y-1, z)->ID == getTile( x, y, z)->ID) {
                    p_vertices[i - 4] = block_vertex( x,     y+1,     z+1, 0);
                    p_vertices[i - 3] = block_vertex( x, y+1, z+1, 0);
                    p_vertices[i - 1] = block_vertex( x+1, y+1, z+1, 0);
                } else {
                    Side_Textur_Pos = List->GetTexturByType( type, 3);

                    p_vertices.resize( p_vertices.size() + 6);
                    p_data.resize( p_data.size() + 6);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x, y + 1, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y, z + 1, 0);

                    p_data[i] = block_data( 0, Side_Textur_Pos.x, Side_Textur_Pos.y, 0);
                    p_vertices[i++] = block_vertex(x + 1, y + 1, z + 1, 0);
                }
                b_visibility = true;
            }

        }
    }
    // normal errechnen
    int v;
    p_normal.resize( i);
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

        p_normal[v] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+1] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+1] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+1] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+2] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+2] = block_data( normal.x, normal.y, normal.z, 0);
        p_normal[v+2] = block_data( normal.x, normal.y, normal.z, 0);
    }
    p_elements = i;
    p_changed = false;
    p_updateonce = true;
    if( p_elements == 0) {// Kein Speicher resavieren weil leer
        return;
    }
    p_arraychange = true;

    p_rigidBody = makeBulletMesh();

    printf( "UpdateArray %dms %d %d %d\n", timer.GetTicks(), p_elements, getAmount());
}

void Chunk::DestoryVbo() {
    p_nomorevbo = true;
    while( p_updatevbo);

    if( p_createvbo ) {
        p_createvbo = false;
        glDeleteBuffers(1, &p_vboVertex);
        glDeleteBuffers(1, &p_vboNormal);
        glDeleteBuffers(1, &p_vboData);
        glDeleteVertexArrays( 1, &p_vboVao);
    }
}

void Chunk::updateVbo( Shader *shader) {
    Timer timer;
    timer.Start();

    // Nicht bearbeiten falls es anderweilig bearbeitet wird
    if( p_deleting)
        return;
    if( p_elements == 0 || p_updateonce == false || p_nomorevbo == true)
        return;

    // VBO erstellen falls dieser fehlt
    if(p_createvbo == false) {
        // create vao
        glGenVertexArrays(1, &p_vboVao);
        // Create vbo
        glGenBuffers(1, &p_vboVertex);
        glGenBuffers(1, &p_vboNormal);
        glGenBuffers(1, &p_vboData);
        p_createvbo = true;
    }

    // flags ändern
    p_arraychange = false;
    p_updatevboonce = true;

    // anderweilig beschäftigt?
    while( p_updatevbo);
    p_updatevbo = true;

    // vbo updaten
    // vertex
    glBindBuffer(GL_ARRAY_BUFFER, p_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, p_elements * sizeof(block_vertex), &p_vertices[0], GL_STATIC_DRAW);
    // normal
    glBindBuffer(GL_ARRAY_BUFFER, p_vboNormal);
    glBufferData(GL_ARRAY_BUFFER, p_elements * sizeof(block_vertex), &p_normal[0], GL_STATIC_DRAW);
    // data
    glBindBuffer(GL_ARRAY_BUFFER, p_vboData);
    glBufferData(GL_ARRAY_BUFFER, p_elements * sizeof(block_data), &p_data[0], GL_STATIC_DRAW);

    // VAO
    glBindVertexArray( p_vboVao);
    // vertex
    glEnableVertexAttribArray(0);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboVertex);
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glBindBuffer( GL_ARRAY_BUFFER, p_vboNormal);
    glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // data
    glEnableVertexAttribArray( shader->getAntribute( 3));
    glBindBuffer( GL_ARRAY_BUFFER, p_vboData);
    glVertexAttribPointer( shader->getAntribute( 3), 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    // print
    p_updatevbo = false;
    printf( "UpdateVbo %dms %d * %d = %d\n", timer.GetTicks(), sizeof(block_vertex), getAmount(), getAmount() * sizeof(block_data));
}

void Chunk::draw( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa) {
    // chunk wird grad gelöscht
    if( p_nomorevbo == true)
        return;
    // wird gelöscht
    if( p_deleting)
        return;
    if( p_updatevbo)
        return;
    p_updatevbo = true;

    // Shader einstellen
    shader->update( p_form.getModel(), viewProjection, aa);

    // use the vao
    glBindVertexArray( p_vboVao);

    // draw the array
    glDrawArrays( GL_TRIANGLES, 0, (int)p_elements);

    // disable
    glBindVertexArray( 0);

    // update war erfolgreich
    p_updatevbo = false;


}
