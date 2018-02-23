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
    Chunk *getChunkWithPos( int x, int y, int z);
    void SetTile( Chunk *chunk, int tile_x, int tile_y, int tile_z, int ID);

    void process_thrend();
    void process();

    void deleteChunks( Chunk* chunk);
    void deleteChunk( Chunk* node);




    Chunk *createChunk( int pos_x, int pos_y, int pos_z);
    void destoryChunk( int pos_x, int pos_y, int pos_z);
    bool CheckChunk( int pos_x, int pos_y, int pos_z);
    Chunk* getChunk( int X, int Y, int Z);
    void addChunk( glm::tvec3<int> pos );
    void addDeleteChunk( glm::tvec3<int> pos );

    void draw( graphic *graphic, Config *config, glm::mat4 viewProjection) ;
    void drawTransparency( Shader* shader, glm::mat4 viewProjection, bool alpha_cutoff, glm::mat4 aa = glm::mat4(1));
    void drawNode( Shader* shader, glm::mat4 viewProjection,  glm::mat4 aa =  glm::mat4(1));

    void UpdateArray();
    void UpdateArrayNode();

    bool getDestory() { return p_destroy; }
    inline int GetAmountChunks() const { return p_chunk_amount; }
protected:
private:
    int Seed;
    bool p_buysvector;
    bool p_world_tree_empty;
    int p_chunk_amount;
    texture *p_tilemap;
    Chunk* Chunks;
    BlockList *p_blocklist;
    bool p_destroy;

    SDL_Thread *p_thread;

    std::vector<glm::tvec3<int>> p_creatingList;
    std::vector<glm::tvec3<int>> p_deletingList;
    std::vector<Chunk*> p_updateNodeList;

    //ShadowMap Shadow;
};

#endif // WORLD_H
