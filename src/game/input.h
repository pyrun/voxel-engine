#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "../graphic/graphic.h"

class InputMap {
public:
    glm::vec2 MousePos;

    bool Up;
    bool Down;
    bool Right;
    bool Left;

    bool Inventory;
    bool Jump;
    bool Shift;

    bool Place;
    bool Destory;

    bool MouseGrab;
protected:
private:
};

class Input {
public:
    Input();
    virtual ~Input();

    void Reset();
    bool Handle( int l_width, int l_height, SDL_Window* l_window);

    InputMap Map;
    InputMap MapOld;
    bool getResize() { return m_resize; }
    int getResizeW() { return m_resize_w; }
    int getResizeH() { return m_resize_h; }
protected:
private:
    bool GrabMouse;
    SDL_Event m_event;
    bool m_resize;
    int m_resize_w;
    int m_resize_h;
};

#endif // INPUT_H
