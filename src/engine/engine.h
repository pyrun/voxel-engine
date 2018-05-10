#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "../graphic/graphic.h"
#include "../graphic/debug_draw.h"
#include "block.h"
#include "input.h"
#include "world.h"
#include "../engine/config.h"
#include "../engine/openvr.h"
#include "../engine/network.h"

struct clock
{
    uint32_t last_tick_time = 0;
    uint32_t delta = 0;

    void tick()
    {
        uint32_t tick_time = SDL_GetTicks();
        delta = tick_time - last_tick_time;
        last_tick_time = tick_time;
    }
};

class engine {
public:
    engine();
    virtual ~engine();

    void startVR();
    void raycastView( glm::vec3 position, glm::vec3 lookat, int forward);

    void render( glm::mat4 view, glm::mat4 projection);

    void fly( int l_delta);
    void run();

    //network *getNetwork() { return p_network; }
protected:

    void drawBox( glm::mat4 viewProjection, glm::vec3 pos);
private:
    bool p_isRunnig;
    Input p_input;
    block_list* p_blocklist;
    graphic* p_graphic;
    config *p_config;
    openvr *p_openvr;

    // world
    std::vector<world *> p_worlds;
    world *p_world_player;

    // frame rate
    bool p_framecap;
    int p_timecap;
    std::vector<int> p_framerate;
};


#endif // ENGINE_H
