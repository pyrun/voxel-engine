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
#include "framenrate.h"
#include "world.h"
#include "config.h"
#include "gui.h"
#include "object.h"
#include "../engine/openvr.h"

std::string NumberToString( int Number);

class game {
public:
    game();
    virtual ~game();

    void startVR();

    void viewCurrentBlock( glm::mat4 viewProjection, int view_width);
    void viewCross();

    void render( glm::mat4 viewProjection);

    void Start();
protected:
    void drawBox( glm::mat4 viewProjection, glm::vec3 pos);
private:
    bool p_isRunnig;
    Input p_input;
    BlockList* p_blocklist;
    graphic* p_graphic;
    Framenrate framenrate;
    Config p_config;
    Gui *p_gui;
    openvr *p_openvr;

    std::string Title;
    World *p_world;
    GLuint p_vboCursor;
};


#endif // GAME_H
