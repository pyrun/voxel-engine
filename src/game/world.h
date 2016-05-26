#ifndef WORLD_H
#define WORLD_H 1

#include "../graphic/graphic.h"
#include "chunk.h"
#include "block.h"
#include "landscape_generator.h"
#include "config.h"
//#include "../graphic/shadowmap.h"

#define WORLD_TILE_IDLE_TIME 2*1000 //2s

#define WORLD_TEST_FACTOR 0

Uint32 thrend_WorldGenerator( Uint32 interval, void *Paramenter);

class World_Position {
public:
    int x;
    int y;
    int z;
    bool operator< ( const World_Position& rhs) {
        if( x + y + z > rhs.x + rhs.y + rhs.z)
            return true;
        return false;
    }
    bool operator> (const World_Position& rhs){
        if( x + y + z < rhs.x + rhs.y + rhs.z)
            return true;
        return false;
    }
    bool operator==(const World_Position& r) {
        if( x == r.x)
            if( y  == r.y)
                if( z == r.z)
                    return true;
        return false;
    }
};

class World {
public:
    World( std::string Tileset, BlockList* B_List);
    virtual ~World();

    Tile *GetTile( int x, int y, int z);
    Chunk *GetChunkWithPos( int x, int y, int z);
    void SetTile( Chunk *chunk, int tile_x, int tile_y, int tile_z, int ID);
    void Process();
    void DeleteChunks( Chunk* chunk);
    inline int GetAmountChunks() const { return m_chunk_amount; }
    inline bool GetUpdate() const { return (int)CreateChunkList.size() != 0;}
    void AddChunk( int X, int Y, int Z);
    void DeletingChunk( int pos_x, int pos_y, int pos_z);
    void CreateChunk( int pos_x, int pos_y, int pos_z);
    void DestoryChunk( int pos_x, int pos_y, int pos_z);
    bool CheckChunk( int pos_x, int pos_y, int pos_z);
    Chunk* GetChunk( int X, int Y, int Z);
    void Draw( Graphic *graphic, Config *config) ;
    void DrawTransparency( Graphic* graphic, Shader* shader, Camera* camera, Camera* shadow, bool alpha_cutoff, glm::mat4 aa = glm::mat4(1));
    void DrawNode( Graphic* graphic, Shader* shader, Camera* camera,  Camera* shadow,  glm::mat4 aa =  glm::mat4(1));
    void UpdateArray();
    void UpdateArrayNode();
    int GetDeletingObjects() { return DeletingChunkList.size(); }
protected:
private:
    int Seed;
    bool m_buysvector;
    bool m_world_tree_empty;
    int m_chunk_amount;
    Texture *Tilemap;
    std::vector<World_Position> CreateChunkList;
    std::vector<World_Position> DeletingChunkList;
    Chunk* Chunks;
    BlockList *m_blocklist;

    //ShadowMap Shadow;
};

#endif // WORLD_H
