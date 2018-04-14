#ifndef WORLD_H
#define WORLD_H 1

#include <bounce/bounce.h>

#include "../graphic/graphic.h"
#include "chunk.h"
#include "block.h"
#include "landscape_generator.h"
#include "../engine/config.h"
#include "../system/timer.h"
#include "../graphic/debug_draw.h"

#define WORLD_TILE_IDLE_TIME 2*1000 //2s

#define WORLD_UPDATE_THRENDS 6

#define WORLD_PHYSIC_FIXED_TIMESTEP ( 1.0 / 60.0 )

Uint32 thrend_worldGenerator( Uint32 interval, void *Paramenter);

class world_data_list {
    public:
        glm::vec3 position;
        bool landscape;
};

class world {
public:
    world( texture *image, block_list* B_List);
    virtual ~world();

    int GetTile( int x, int y, int z);
    Chunk *getChunkWithPos( int x, int y, int z);
    void SetTile( Chunk *chunk, int tile_x, int tile_y, int tile_z, int ID);

    void process_thrend_handle();
    void process_thrend_update();
    void process_thrend_physic();

    void deleteChunks( Chunk* chunk);
    void deleteChunk( Chunk* node);
    Chunk *createChunk( int pos_x, int pos_y, int pos_z, bool generateLandscape = false, bool update = true);

    bool CheckChunk( int pos_x, int pos_y, int pos_z);
    Chunk* getChunk( glm::vec3 position);

    void addChunk( glm::tvec3<int> pos, bool generateLandscape);
    void addDeleteChunk( glm::tvec3<int> pos );

    void draw( graphic *graphic, Shader *shader, glm::mat4 view, glm::mat4 projection);
    void drawNode( Shader* shader, glm::mat4 view, glm::mat4 projection);

    bool getDestory() { return p_destroy; }
    inline int getAmountChunks() const { return p_chunk_amount; }
    Chunk *getNode() { return p_chunk_start; }
    SDL_mutex *getMutex() { return p_mutex; }
    b3World *getPhysicWorld() { return p_physicScene; }
protected:
private:
    bool p_buysvector;
    bool p_world_tree_empty;

    b3World *p_physicScene;

    int p_chunk_amount;
    Chunk* p_chunk_start;
    Chunk* p_chunk_last;
    block_list *p_blocklist;
    bool p_destroy;
    texture *p_image;

    SDL_mutex *p_mutex;
    SDL_mutex *p_mutex_physic;
    SDL_Thread *p_thread_update[WORLD_UPDATE_THRENDS];
    SDL_Thread *p_thread_handle;
    SDL_Thread *p_thread_physic;

    std::vector<world_data_list> p_creatingList;
    std::vector<world_data_list> p_deletingList;

    //ShadowMap Shadow;
    float p_time;

    debug_draw p_renderer;
};

#endif // WORLD_H
