#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "landscape_generator.h"
#include "../graphic/graphic.h"
#include "../graphic/debug_draw.h"
#include "block.h"
#include "input.h"
#include "world.h"
#include "player.h"
#include "script.h"
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
    void startServer();
    void startClient( std::string address = "127.0.0.1");

    void install_lua();

    void render( glm::mat4 view, glm::mat4 projection);

    void fly( int l_delta);
    void walk( int l_delta);

    world *createWorld( std::string name, bool player);
    void loadWorld( std::string name, bool player = false);
    world *getWorld( std::string name);

    void run();

    graphic *getGraphic() { return p_graphic; }
    block_list *getBlocklist() { return p_blocklist; }
    network *getNetwork() { return p_network; }
    player *getPlayer() { return p_player; }
    player_handle *getPlayerHandle() { return p_players; }
protected:

    void drawBox( glm::mat4 viewProjection, glm::vec3 pos);
private:
    bool p_isRunnig;
    Input p_input;
    block_list* p_blocklist;
    graphic* p_graphic;
    config *p_config;
    openvr *p_openvr;
    landscape *p_landscape_generator;
    network *p_network;

    // objects
    object_handle *p_object_handle;

    // world
    std::vector<world *> p_worlds;

    std::string p_player_file;

    // player
    player *p_player;
    player_handle *p_players;

    // frame rate
    bool p_framecap;
    uint32_t p_timecap;
    std::vector<int> p_framerate;
};


#endif // ENGINE_H
