#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "../graphic/graphic.h"
#include "block.h"
#include "input.h"
#include "world.h"
#include "../engine/config.h"
#include "gui.h"
#include "../engine/openvr.h"
#include "../engine/network.h"

std::string NumberToString( int Number);

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

class game {
public:
    game();
    virtual ~game();

    void startVR();

    void viewCurrentBlock( glm::mat4 viewProjection, int view_width);
    void viewCross();

    void render( glm::mat4 viewProjection);

    void Start();

    network *getNetwork() { return p_network; }
protected:

    void drawBox( glm::mat4 viewProjection, glm::vec3 pos);
private:
    bool p_isRunnig;
    Input p_input;
    block_list* p_blocklist;
    graphic* p_graphic;
    config *p_config;
    Gui *p_gui;
    openvr *p_openvr;
    network *p_network;

    texture *p_tilemap;

    std::string Title;
    GLuint p_vboCursor;

    bool p_framecap;
    int p_timecap;
    std::vector<int> p_framerate;
};


#endif // GAME_H
