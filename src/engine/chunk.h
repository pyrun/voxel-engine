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

#define CHUNK_SIZE 16
#define CHUNK_SCALE 2.0f

#define EMPTY_BLOCK_ID 0

#define TILE_REGISTER( posX, posY, posZ)  posX + CHUNK_SIZE * (posY + CHUNK_SIZE * posZ) //Z*CHUNK_DEPTH*CHUNK_WIDTH + X*CHUNK_WIDTH + Y

struct tile {
    int ID;
};

#define TILE_VERTEX_NULL -1

typedef glm::tvec4<GLfloat> block_vertex;
typedef glm::tvec4<GLfloat> block_data;

//static float Noise2d(float x, float y, int seed, int octaves, float persistence);
//static float Noise3d(float x, float y, float z, int seed, int octaves, float persistence);

class Chunk {
public:
    Chunk( int X, int Y, int Z, int Seed, block_list* b_list);
    virtual ~Chunk();

    Chunk *front;
    Chunk *back;
    Chunk *right;
    Chunk *left;
    Chunk *up;
    Chunk *down;
    Chunk *next;

    btRigidBody *makeBulletMesh( btDiscreteDynamicsWorld *world);

    void serialize(bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end)
    {
        // CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE
        for( int i = start; i < end; i++)
            bitstream->Serialize( writeToBitstream, p_tile[i].ID);
    }

    inline glm::vec3 getPos() { return p_pos; }

    inline int getAmount() { return p_vertices.size(); }

    void CreateTile( int X, int Y, int Z, int ID);
    void set( int X, int Y, int Z, int ID);
    tile *getTile( int X, int Y, int Z);
    bool CheckTile( int X, int Y, int Z);

    void updateForm();

    void updateArray( block_list *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void DestoryVbo();
    void updateVbo( Shader *shader);
    void draw( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa = glm::mat4(1));

    tile getArray( int i) { return p_tile[i]; }
    int getSizeofArray() { return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; }

    btRigidBody *getPhysicBody() { return p_rigidBody; }

    GLuint getVbo() { return p_vboVertex; }
    void changed( bool set) { p_changed = set; }
protected:
private:
    glm::tvec3<int> p_pos;

    Transform p_form;

    int p_elements;

    bool p_changed;
    bool p_updateVbo;
    bool p_updateRigidBody;

    GLuint p_vboVao;
    GLuint p_vboVertex;
    GLuint p_vboNormal;
    GLuint p_vboData;

    tile* p_tile;

    btRigidBody *p_rigidBody;

    std::vector<block_vertex> p_vertices;
    std::vector<block_vertex> p_normal;
    std::vector<block_vertex> p_data;
    //block_data p_data[ CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * 6 * 6];

    /*std::vector<ChunkVboVertexStruct> p_vertices;
    std::vector<ChunkVboDataStruct> p_data;*/
};

#endif // CHUNK_H
