#ifndef GAME_H
#define GAME_H

#include <SDL2\SDL.h>
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
    bool m_isRunnig;
    Input m_input;
    BlockList* m_blocklist;
    Graphic* m_graphic;
    Framenrate framenrate;
    Config m_config;
    Gui *m_gui;
    Sun *m_sun;

    std::string Title;
    World *m_world;
    GLuint m_vboCursor;
};


#endif // GAME_H
