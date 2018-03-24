#ifndef CHUNK_H
#define CHUNK_H 1

#include <string.h>
#include <vector>

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "btBulletDynamicsCommon.h"
#include <BulletCollision/Gimpact/btGImpactShape.h>
#include "BulletCollision/CollisionShapes/btShapeHull.h"

#include "../graphic/graphic.h"
#include "block.h"

#define CHUNK_SIZE 32
#define CHUNK_SCALE 1.0f

#define EMPTY_BLOCK_ID 0

#define MAX_TILE_ID 32.767 // dont have more blocks then this

#define TILE_REGISTER( posX, posY, posZ)  posX + CHUNK_SIZE * (posY + CHUNK_SIZE * posZ) //Z*CHUNK_DEPTH*CHUNK_WIDTH + X*CHUNK_WIDTH + Y

class Chunk {
public:
    Chunk( int X, int Y, int Z, int Seed);
    virtual ~Chunk();

    Chunk *front;
    Chunk *back;
    Chunk *right;
    Chunk *left;
    Chunk *up;
    Chunk *down;
    Chunk *next;

    btRigidBody *makeBulletMesh( btDiscreteDynamicsWorld *world);

    void serialize(bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end, block_list *blocks)
    {
        for( int i = start; i < end; i++) {
            bitstream->Serialize( writeToBitstream, p_tile[i]);
            if( p_tile[i] > MAX_TILE_ID) {
                printf( "chunk::serialize corrupt data x%d y%d z%d Data %d to %d tileID#%d\n", (int)p_pos.x, (int)p_pos.y, (int)p_pos.z, start, end, p_tile[i]);
                p_tile[i] = EMPTY_BLOCK_ID;
                return;
            }
            if(!writeToBitstream) {
                if( blocks->get( p_tile[i]) == NULL )
                    p_tile[i] = 0;
            }
        }
    }

    inline glm::vec3 getPos() { return p_pos; }

    inline int getAmount() { return p_vertices.size(); }
    void set( int X, int Y, int Z, int ID, bool change = true);
    int getTile( int X, int Y, int Z);
    bool CheckTile( int X, int Y, int Z);

    void updateForm();

    void addFaceX( bool flip, glm::vec3 pos, glm::vec3 data);
    void addFaceY( bool flip, glm::vec3 pos, glm::vec3 data);
    void addFaceZ( bool flip, glm::vec3 pos, glm::vec3 data);

    void updateArray( block_list *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void DestoryVbo();
    void updateVbo( Shader *shader);
    void draw( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa = glm::mat4(1));

    int getArray( int i) { return p_tile[i]; }
    int getSizeofArray() { return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; }

    btRigidBody *getPhysicBody() { return p_rigidBody; }

    GLuint getVbo() { return p_vboVertex; }
    void changed( bool set) { p_changed = set; }
    bool isChanged() { return p_changed; }
protected:
private:
    glm::tvec3<int> p_pos;

    Transform p_form;

    int p_elements;

    bool p_changed;
    bool p_updateVbo;
    bool p_updateRigidBody;

    GLuint p_vboVao;
    GLuint p_vboIndex;
    GLuint p_vboVertex;
    GLuint p_vboNormal;
    GLuint p_vboData;

    int* p_tile;

    btRigidBody *p_rigidBody;

    std::vector<unsigned int> p_indices;
    std::vector<glm::vec3> p_vertices;
    std::vector<glm::vec3> p_normal;
    std::vector<glm::vec3> p_data;

    /*std::vector<ChunkVboVertexStruct> p_vertices;
    std::vector<ChunkVboDataStruct> p_data;*/
};

#endif // CHUNK_H
