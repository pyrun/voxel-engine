#ifndef CHUNK_H
#define CHUNK_H 1

#include <string.h>
#include <vector>

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "btBulletDynamicsCommon.h"
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

    btRigidBody *makeBulletMesh();

    bool SetDeleting() {
        if( p_deleting)
            return false;
        p_deleting = true;
        return true;
    }

    void serialize(bool writeToBitstream, RakNet::BitStream *bitstream)
    {
        for( int i = 0; i < CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; i++)
            bitstream->Serialize( writeToBitstream, p_tile[i]);
    }

    inline int getX() { return p_pos.x; }
    inline int getY() { return p_pos.y; }
    inline int getZ() { return p_pos.z; }
    inline glm::vec3 getPos() { return p_pos; }

    inline bool GetChanged() { return p_changed; }
    inline bool SetChange( bool Change) { p_changed = Change; return Change; }
    inline bool GetUpdateOnce() { return p_updateonce; }
    inline bool GetVbo() { return p_createvbo; }
    inline bool GetArrayChange() { return p_arraychange; }
    inline int getAmount() { return p_vertices.size(); }
    inline bool GetUpdateVboOnce() { return p_updatevboonce; }
    inline int GetTimeIdle() { return p_time_idle; }
    void ResetTimeIdle() { p_time_idle = SDL_GetTicks(); }

    void CreateTile( int X, int Y, int Z, int ID);
    void set( int X, int Y, int Z, int ID);
    tile *getTile( int X, int Y, int Z);
    bool CheckTile( int X, int Y, int Z);

    void updateForm();

    void UpdateArray( block_list *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void DestoryVbo();
    void updateVbo( Shader *shader);
    void draw( Shader* shader, glm::mat4 viewProjection, glm::mat4 aa = glm::mat4(1));

    tile getArray( int i) { return p_tile[i]; }
    int getSizeofArray() { return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; }

    void setPhysic( bool set) { p_physicSet = set; }
    bool isPhysicSet() { return p_physicSet; }
    btRigidBody *getPhysicBody() { return p_rigidBody; }
protected:
private:
    glm::tvec3<int> p_pos;
    Transform p_form;
    int p_time_idle;
    int p_elements;
    bool p_changed;
    bool p_updateonce;
    bool p_createvbo;
    bool p_arraychange;
    bool p_updatevboonce;
    bool p_updatevbo;
    bool p_nomorevbo;
    bool p_deleting;
    GLuint p_vboVao;
    GLuint p_vboVertex;
    GLuint p_vboNormal;
    GLuint p_vboData;
    tile* p_tile;
    int p_seed;
    bool p_physicSet;
    btRigidBody *p_rigidBody;

    std::vector<block_vertex> p_vertices;
    std::vector<block_vertex> p_normal;
    std::vector<block_vertex> p_data;
    //block_data p_data[ CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * 6 * 6];

    /*std::vector<ChunkVboVertexStruct> p_vertices;
    std::vector<ChunkVboDataStruct> p_data;*/
};

#endif // CHUNK_H
