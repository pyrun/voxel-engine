#ifndef CHUNK_H
#define CHUNK_H 1

#include <bounce/bounce.h>
#include <string.h>
#include <vector>

#include "RakPeerInterface.h"
#include "BitStream.h"

#include "../graphic/graphic.h"
#include "block.h"

#define CHUNK_SIZE 32
#define CHUNK_SCALE 1.0f

#define EMPTY_BLOCK_ID 0

#define MAX_TILE_ID 32767 // dont have more blocks then this

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

    bool createPhysicBody( b3World *world);

    bool serialize(bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end, block_list *blocks);

    inline glm::vec3 getPos() { return p_pos; }

    inline int getAmount() { return p_vertices.size(); }
    void set( int X, int Y, int Z, int ID, bool change = true);
    unsigned short getTile( int X, int Y, int Z);
    bool checkTile( int x, int y, int z);

    int getSunlight(int x, int y, int z);
    void setSunlight(int x, int y, int z, int val);
    int getTorchlight(int x, int y, int z);
    void setTorchlight(int x, int y, int z, int val);

    void addFaceX( bool flip, glm::vec3 pos, glm::vec3 data, glm::vec3 blockPos);
    void addFaceY( bool flip, glm::vec3 pos, glm::vec3 data, glm::vec3 blockPos);
    void addFaceZ( bool flip, glm::vec3 pos, glm::vec3 data, glm::vec3 blockPos);

    void updateArray( block_list *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void updateForm();
    void updateVbo();

    void draw( Shader* shader);

    int getArray( int i) { return p_tile[i]; }
    int getSizeofArray() { return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; }

    GLuint getVbo() { return p_vboVertex; }
    void changed( bool set) { p_changed = set; }
    bool isChanged() { return p_changed; }

    b3Body* getBody() { return p_body; }
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
    GLuint p_vboLight;

    unsigned short* p_tile;
    unsigned char* p_lighting;

    b3Body* p_body;
    b3ShapeDef *p_shape;
    b3Mesh *p_mesh;

    // side
    std::vector<unsigned int> p_indices;

    std::vector<glm::vec3> p_vertices;
    std::vector<glm::vec3> p_normal;
    std::vector<glm::vec3> p_texture;
    std::vector<glm::vec3> p_light;

    /*std::vector<ChunkVboVertexStruct> p_vertices;
    std::vector<ChunkVboDataStruct> p_data;*/
};

struct ChunkLightNode {
    ChunkLightNode(short indx, Chunk* ch) : index(indx), chunk(ch) {}

    short index; //this is the x y z coordinate!

    Chunk* chunk; //pointer to the chunk that owns it!
};

#endif // CHUNK_H
