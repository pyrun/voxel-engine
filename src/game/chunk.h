#ifndef CHUNK_H
#define CHUNK_H 1

#include <vector>
#include "../graphic/graphic.h"
#include "block.h"

#define CHUNK_WIDTH 128
#define CHUNK_HEIGHT 128
#define CHUNK_DEPTH 128

#define TILE_REGISTER( posX, posY, posZ)  posX + CHUNK_WIDTH * (posY + CHUNK_DEPTH * posZ) //Z*CHUNK_DEPTH*CHUNK_WIDTH + X*CHUNK_WIDTH + Y

struct Tile {
    int ID;
    short m_left;
    short m_right;
    short m_up;
    short m_down;
    short m_forward;
    short m_back;
};

#define TILE_VERTEX_NULL -1

struct ChunkVboDataStruct {
    GLshort x;
    GLshort y;
    GLshort z;
    GLshort w;
};

struct ChunkVboVertexStruct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
};


/** \brief Erstellt ein Vbo datenblock in einem format der Grafikkarte
 *
 * \param gl float X
 * \param gl float Y
 * \param gl float Z
 * \return Vbo daten block
 *
 */
ChunkVboDataStruct CVDS_SetBlock( GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f, GLfloat w = 0.0f);


static float Noise2d(float x, float y, int seed, int octaves, float persistence);
static float Noise3d(float x, float y, float z, int seed, int octaves, float persistence);

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
        if( m_deleting)
            return false;
        m_deleting = true;
        return true;
    }

    inline int GetX() { return x; }
    inline int GetY() { return y; }
    inline int GetZ() { return z; }

    inline bool GetChanged() { return m_changed; }
    inline bool SetChange( bool Change) { m_changed = Change; return Change; }
    inline bool GetUpdateOnce() { return m_updateonce; }
    inline bool GetVbo() { return m_createvbo; }
    inline bool GetArrayChange() { return m_arraychange; }
    inline int GetAmount() { return m_vertex.size(); }
    inline bool GetUpdateVboOnce() { return m_updatevboonce; }
    inline int GetTimeIdle() { return m_time_idle; }
    void ResetTimeIdle() { m_time_idle = SDL_GetTicks(); }

    void CreateTile( int X, int Y, int Z, int ID);
    Tile *GetTile( int X, int Y, int Z);
    bool CheckTile( int X, int Y, int Z);

    void UpdateArray( BlockList *List, Chunk *Back = NULL, Chunk *Front = NULL, Chunk *Left = NULL, Chunk *Right = NULL, Chunk *Up = NULL, Chunk *Down = NULL);
    void DestoryVbo();
    void UpdateVbo();
    void Draw( Graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, glm::mat4 aa = glm::mat4(1));
protected:
private:
    int x;
    int y;
    int z;
    int m_time_idle;
    int m_elements;
    bool m_changed;
    bool m_updateonce;
    bool m_createvbo;
    bool m_arraychange;
    bool m_updatevboonce;
    bool m_updatevbo;
    bool m_nomorevbo;
    bool m_deleting;
    GLuint m_vboVertex;
//    GLuint m_vboNormal;
    GLuint m_vboData;
    Tile** m_tile;
    int m_seed;
    std::vector<ChunkVboVertexStruct> m_vertex;
//    std::vector<ChunkVboDataStruct> m_normal;
    std::vector<ChunkVboDataStruct> m_data;
};

#endif // CHUNK_H
