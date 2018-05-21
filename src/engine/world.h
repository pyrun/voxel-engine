#ifndef WORLD_H
#define WORLD_H 1

#include <queue>
#include <bounce/bounce.h>

#include "../graphic/graphic.h"
#include "chunk.h"
#include "block.h"
#include "object.h"
#include "landscape_generator.h"
#include "../engine/config.h"
#include "../system/timer.h"
#include "../graphic/debug_draw.h"

#define WORLD_HANDLE_THRENDS 1
#define WORLD_UPDATE_THRENDS 3

#define WORLD_PHYSIC_FIXED_TIMESTEP ( 1.0f / 120.0f )

Uint32 thrend_worldGenerator( Uint32 interval, void *Paramenter);

class world_physic: public b3World
{
    public:
        void setDebugDraw( b3Draw *draw )  {
            this->m_debugDraw = draw;
        }

    private:
        b3Draw* m_debugDraw;
};

class world_data_list {
    public:
        world_data_list( glm::ivec3 position, bool landscape) {
            this->position = position;
            this->landscape = landscape;
        }

        glm::ivec3 position;
        bool landscape;
};

class world_change_block {
    public:
        Chunk *chunk;
        glm::ivec3 position;
        int id;
};

class world_light_node {
    public:
        world_light_node( glm::vec3 position, Chunk* chunk, short strength = 0) {
            this->position = position;
            this->chunk = chunk;
            this->strength = strength;
        }

        glm::ivec3 position;
        Chunk* chunk;
        short strength; // just use for deleting
};

class world {
public:
    world( block_list* block_list, std::string name, object_handle *objectHandle);
    virtual ~world();

    int createObject( std::string name, glm::vec3 position);
    object *getObject( int id);

    int getTile( glm::ivec3 position);
    Chunk *getChunkWithPosition( glm::ivec3 position);
    void changeBlock( Chunk *chunk, glm::vec3 position, int id);
    void setTile( Chunk *chunk, glm::ivec3 position, int id);

    void addTorchlight( Chunk *chunk, glm::ivec3 position, int value);
    void delTorchlight( Chunk *chunk, glm::ivec3 position);

    void process_thrend_handle();
    void process_thrend_update();
    void process_thrend_physic();
    void process_object_handling();

    void deleteChunks( Chunk* chunk);
    void deleteChunk( Chunk* node);
    Chunk *createChunk( glm::ivec3 position);

    bool CheckChunk( int pos_x, int pos_y, int pos_z);
    Chunk* getChunk( glm::ivec3 position);

    void addChunk( glm::ivec3 position, bool generateLandscape);
    void addDeleteChunk( glm::ivec3 position );

    void drawObjects( graphic *graphic, Shader *shader);
    void drawVoxel( graphic *graphic, Shader *shader);
    void drawNode( Shader* shader);

    bool fileExists(std::string filename);
    void save();
    bool load();
    void load_chunk( std::string path_file);

    bool getDestory() { return p_destroy; }
    inline int getAmountChunks() const { return p_chunk_amount; }
    inline int getAmountChunksVisible() const { return p_chunk_visible_amount; }
    Chunk *getNode() { return p_chunk_start; }
    world_physic *getPhysicWorld() { return p_physicScene; }

    void setGenerator( landscape *generator) { p_landscape_generator = generator; }
    void DebugDraw( Shader *shader) { p_renderer.draw( *p_physicScene, shader); }
protected:
private:
    std::string p_name;

    landscape *p_landscape_generator;

    bool p_buysvector;
    bool p_world_tree_empty;

    world_physic *p_physicScene;

    int p_chunk_amount;
    int p_chunk_visible_amount;
    Chunk* p_chunk_start;
    Chunk* p_chunk_last;
    block_list *p_blocklist;
    bool p_destroy;

    SDL_mutex *p_mutex_handle;
    SDL_mutex *p_mutex_physic;
    SDL_Thread *p_thread_update[WORLD_UPDATE_THRENDS];
    SDL_Thread *p_thread_handle[WORLD_HANDLE_THRENDS];
    SDL_Thread *p_thread_physic;

    std::queue<world_data_list> p_creatingList;
    std::queue<world_data_list> p_deletingList;
    std::queue<world_data_list> p_landscape;
    std::queue<world_change_block> p_change_blocks;
    std::queue<world_data_list> p_update_changes;

    std::queue <world_light_node> p_lightsAdd;
    std::queue <world_light_node> p_lightsDel;

    object_handle *p_pointer_object_handle;
    std::vector<object*> p_objects;
    int p_object_id;

    float p_time;

    debug_draw p_renderer;
};

#endif // WORLD_H
