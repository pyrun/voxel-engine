#ifndef LANDSCAPE_GENERATOR_H
#define LANDSCAPE_GENERATOR_H

#include "block.h"
#include "chunk.h"
#include "config.h"

#include <lua.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

class landscape_script{
    public:
        landscape_script( int id, lua_State *state);
        ~landscape_script();

        void setState( lua_State *state);

        std::vector<glm::ivec3> generator( Chunk* chunk, block_list* blocklist);
    private:
        int p_id;
        lua_State* p_luastate;
};

class landscape{
    public:
        landscape( config *configuration);
        ~landscape();

        landscape_script *getGenerator( Chunk *chunk);

    private:
        bool fileExists(std::string filename);

        bool loadScripts( std::string path);
        bool loadScript( std::string file);

    private:
        int p_id;
        std::vector<landscape_script*> p_generator;
};

std::vector<glm::ivec3> Landscape_Generator( Chunk* chunk, block_list* list);

#endif // LANDSCAPE_GENERATOR_H
