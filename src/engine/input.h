#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "../graphic/graphic.h"

namespace input {

    class mapping {
    public:
        glm::vec2 mouse_position;

        bool up;
        bool down;
        bool right;
        bool left;

        bool inventory;
        bool jump;
        bool shift;

        bool place;
        bool destory;

        bool mouse_grab;

        bool refresh;
    protected:
    private:
    };

    class handle {
    public:
        handle();
        virtual ~handle();

        void reset();
        bool process( int l_width, int l_height, SDL_Window* l_window);
        bool process();

        mapping mappping;
        mapping mappping_previously;

        bool getResize() { return p_resize; }
        int getResizeW() { return p_resize_w; }
        int getResizeH() { return p_resize_h; }
    protected:
    private:
        bool p_grab_mouse;
        SDL_Event p_event;
        bool p_resize;
        int p_resize_w;
        int p_resize_h;
    };

};

#endif // INPUT_H
