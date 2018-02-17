#ifndef CHUNK_H
#define CHUNK_H 1

#include <vector>
#include "../graphic/graphic.h"
#include "block.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 16

#define EMPTY_BLOCK_ID 0

#define TILE_REGISTER( posX, posY, posZ)  posX + CHUNK_WIDTH * (posY + CHUNK_DEPTH * posZ) //Z*CHUNK_DEPTH*CHUNK_WIDTH + X*CHUNK_WIDTH + Y

struct Tile {
    int ID;
};

#define TILE_VERTEX_NULL -1

typedef glm::tvec4<GLfloat> block_vertex;
typedef glm::tvec4<GLshort> block_data;

//static float Noise2d(float x, float y, int seed, int octaves, float persistence);
//static float Noise3d(float x, float y, float z, int seed, int octaves, float persistence);

class Chunk {
public:
    Chunk( int X, int Y, int Z, int Seed, BlockList* b_list);
    virtual ~Chunk();

    Chunk *front;
    Chunk *back;
    Chunk *right;
    Chunk *left;
    Chunk *up;
    Chunk *down;

    Chunk *next;
    bool IsDrawable() {
        /*if( front == NULL)
            return false;
        if( back == NULL)
            return false;
        if( right == NULL)
            return false;
        if( left == NULL)
            return false;
        if( up == NULL)
            return false;
        if( down == NULL)
            return false;*/
        return true;
    }

    bool SetDeleting() {
        if( p_deleting)
            return false;
        p_deleting = true;
        return true;
    }

    inline int GetX() { return x; }
    inline int GetY() { return y; }
    inline int GetZ() { return z; }

    inline bool GetChanged() { return p_changed; }
    inline bool SetChange( bool Change) { p_changed = Change; return Change; }
    inline bool GetUpdateOnce() { return p_updateonce; }
    inline bool GetVbo() { return p_createvbo; }
    inline bool GetArrayChange() { return p_arraychange; }
    inline int getAmount() { return /*sizeof(p_vertex)/sizeof(*p_vertex);*/p_vertex.size(); }
    inline bool GetUpdateVboOnce() { return p_updatevboonce; }
    inline int GetTimeIdle() { return p_time_idle; }
    void ResetTimeIdle() { p_time_idle = SDL_GetTicks(); }

    void CreateTile( int X, int Y, int Z, int ID);
    void set( int X, int Y, int Z, int ID);
    Tile *GetTile( int X, int Y, int Z);
    bool CheckTile( int X, int Y, int Z);

    void UpdateArray( BlockList *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void DestoryVbo();
    void UpdateVbo();
    void Draw( graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, glm::mat4 aa = glm::mat4(1));
protected:
private:
    int x;
    int y;
    int z;
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
    GLuint p_vboVertex;
    GLuint p_vboData;
    Tile* p_tile;
    int p_seed;

    std::vector<block_vertex> p_vertex;
    std::vector<block_data> p_data;
    //block_data p_data[ CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * 6 * 6];

    /*std::vector<ChunkVboVertexStruct> p_vertex;
    std::vector<ChunkVboDataStruct> p_data;*/
};

#endif // CHUNK_H
