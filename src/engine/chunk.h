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

#define EMPTY_BLOCK_ID 0

#define MAX_TILE_ID 32767 // dont have more blocks then this

#define TILE_REGISTER( posX, posY, posZ)  posX + CHUNK_SIZE * (posY + CHUNK_SIZE * posZ) //Z*CHUNK_DEPTH*CHUNK_WIDTH + X*CHUNK_WIDTH + Y

#define LIGHTING_MAX 15

enum Chunk_side{
    CHUNK_SIDE_X_POS = 0,
    CHUNK_SIDE_X_NEG,
    CHUNK_SIDE_Y_POS,
    CHUNK_SIDE_Y_NEG,
    CHUNK_SIDE_Z_POS,
    CHUNK_SIDE_Z_NEG,
};

class Chunk {
public:
    Chunk( glm::ivec3 position, int seed);
    virtual ~Chunk();

    // pointer
    Chunk *next;

    bool createPhysicBody( b3World *world, SDL_mutex *mutex);
    bool serialize( bool writeToBitstream, RakNet::BitStream *bitstream, int start, int end, block_list *blocks);
    int getSeed() { return p_seed; };

    inline glm::ivec3 getPos() { return p_pos; }
    void setSide( Chunk *chunk, Chunk_side side);
    Chunk *getSide( Chunk_side side);

    inline int getAmount() { return p_vertices.size(); }
    void set( glm::ivec3 position, int ID, bool change = true);
    unsigned short getTile( int X, int Y, int Z);
    unsigned short getTile( glm::ivec3 position);
    bool checkTile( glm::ivec3 position);

    int getTorchlight( glm::ivec3 position);
    Chunk *setTorchlight( glm::ivec3 position, int val);

    void addFace( Chunk_side side, glm::ivec3 pos, glm::ivec3 texture, glm::ivec3 blockPos);

    void updateArray( block_list *List);
    void updateForm();
    void updateVbo();

    bool draw( Shader* shader);

    int getArray( int i) { return p_tile[i]; }
    int getSizeofArray() { return CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE; }

    GLuint getVbo() { return p_vboVertex; }
    void changed( bool set, bool vbo_changed = false) { p_changed = set; p_updateVbo = vbo_changed; }
    bool isChanged() { return p_changed; }

    b3Body* getBody() { return p_body; }
protected:
private:
    glm::ivec3 p_pos;
    Transform p_form;
    int p_seed;

    int p_elements;

    bool p_changed;
    bool p_at_update;
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

    std::vector<glm::ivec3> p_vertices;
    std::vector<glm::ivec3> p_normal;
    std::vector<glm::ivec3> p_texture;
    std::vector<glm::ivec3> p_light;

    int p_indices_length;
    int p_vertices_length;

    // sides
    Chunk *p_x_pos;
    Chunk *p_x_neg;
    Chunk *p_y_pos;
    Chunk *p_y_neg;
    Chunk *p_z_pos;
    Chunk *p_z_neg;

    /*std::vector<ChunkVboVertexStruct> p_vertices;
    std::vector<ChunkVboDataStruct> p_data;*/
};

struct ChunkLightNode {
    ChunkLightNode(short indx, Chunk* ch) : index(indx), chunk(ch) {}

    short index; //this is the x y z coordinate!

    Chunk* chunk; //pointer to the chunk that owns it!
};

#endif // CHUNK_H
