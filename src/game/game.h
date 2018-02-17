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
#include "sun.h"
#include "object.h"

std::string NumberToString( int Number);

class Game {
public:
    Game();
    virtual ~Game();
    void ViewCurrentBlock( int view_width);
    void ViewCross();
    void Start();
protected:
    void DrawBox( GLshort bx, GLshort by, GLshort bz);
private:
    bool p_isRunnig;
    Input p_input;
    BlockList* p_blocklist;
    Graphic* p_graphic;
    Framenrate framenrate;
    Config p_config;
    Gui *p_gui;
    Sun *p_sun;

    std::string Title;
    World *p_world;
    GLuint p_vboCursor;
};


#endif // GAME_H
