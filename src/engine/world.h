#ifndef WORLD_H
#define WORLD_H 1

#include <queue>

#include "../graphic/graphic.h"
#include "chunk.h"
#include "block.h"
#include "object.h"
#include "physic.h"
#include "landscape_generator.h"
#include "../engine/config.h"
#include "../system/timer.h"
#include "../graphic/debug_draw.h"

#define WORLD_HANDLE_THRENDS 1
#define WORLD_UPDATE_THRENDS 3

#define WORLD_PHYSIC_STEP 0.2f

#define WORLD_PHYSIC_FIXED_TIMESTEP ( 1.0f / 120.0f )

#define WORLD_OBJECT_SYNC 16 // ms

Uint32 thrend_worldGenerator( Uint32 interval, void *Paramenter);

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
    world( block_list* block_list, std::string name, object_handle *objectHandle, bool player = false);
    virtual ~world();

    int createObject( std::string name, glm::vec3 position, unsigned int id = 0, bool call = true);
    object *getObject( int id);
    void deleteObject( int id);

    int getTile( glm::ivec3 position);
    Chunk *getChunkWithPosition( glm::ivec3 position);
    void changeBlock( Chunk *chunk, glm::vec3 position, int id, bool call = true);
    void setTile( Chunk *chunk, glm::ivec3 position, int id);
    void (*changeCall)( world* world, Chunk *chunk, glm::ivec3 position, unsigned int id);
    void (*createObjectCall)( world *world, std::string type, glm::vec3 position, unsigned int id);
    void (*objectSyncCall)( world *world, object *object);
    void (*deleteObjectCall)( world *world, unsigned int id);

    void addTorchlight( Chunk *chunk, glm::ivec3 position, int value);
    void delTorchlight( Chunk *chunk, glm::ivec3 position);
    void caluculationLight();

    void process_thrend_handle();
    void process_thrend_update();
    void process_thrend_physic();
    void process_object_handling();

    void deleteChunks( Chunk* chunk);
    void deleteChunk( Chunk* node);
    Chunk *createChunk( glm::ivec3 position);
    void reloadScripts();

    bool CheckChunk( int pos_x, int pos_y, int pos_z);
    Chunk* getChunk( glm::ivec3 position);

    void addChunk( glm::ivec3 position, bool generateLandscape);
    void addDeleteChunk( glm::ivec3 position );

    void drawObjects( graphic *graphic, Shader *shader);
    void drawObjectsDebug( graphic *graphic, Shader *shader);
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

    void setGenerator( landscape *generator) { p_landscape_generator = generator; }
    void setSpawnPoint( glm::ivec3 position) { p_spawn_point = position; }
    glm::vec3 getSpawnPoint() { return p_spawn_point; }

    bool getPhysicFlag() {
        if( p_physic_flag) {
            p_physic_flag = false;
            return true;
        }
        return p_physic_flag;
    }
    std::string getName() { return p_name; }
    std::vector<object*> getObjects() { return p_objects; }
protected:
private:
    std::string p_name;

    landscape *p_landscape_generator;
    glm::vec3 p_spawn_point;

    bool p_buysvector;
    bool p_world_tree_empty;
    bool p_player_world;

    int p_chunk_amount;
    int p_chunk_visible_amount;

    Chunk* p_chunk_start;
    Chunk* p_chunk_last;

    block_list *p_blocklist;
    bool p_destroy;

    glm::vec3 p_gravity;

    SDL_mutex *p_mutex_handle;
    SDL_mutex *p_mutex_physic;
    SDL_Thread *p_thread_update[WORLD_UPDATE_THRENDS];
    SDL_Thread *p_thread_handle[WORLD_HANDLE_THRENDS];

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
    timer p_snyc_object_timer;

    float p_time;

    bool p_physic_flag;
};

extern void lua_object_install( lua_State *state);
extern void lua_object_set_targets( world *target);

#endif // WORLD_H
