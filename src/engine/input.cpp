#include "input.h"

using namespace input;

handle::handle() {
    // setting the values
    p_grab_mouse = false;
    reset();
    mappping_previously = mappping;
}

handle::~handle() {

}

void handle::reset() {
    // save old mapping
    mappping_previously = mappping;

    // direction arrows
    mappping.up = false;
    mappping.down = false;
    mappping.right = false;
    mappping.left = false;
    // Invetory
    mappping.inventory = false;
    mappping.jump = false;
    mappping.shift = false;

    mappping.place = false;
    mappping.destory = false;

    mappping.mouse_grab = false;

    // special keys
    mappping.refresh = false;
}

bool handle::process( int l_width, int l_height, SDL_Window* l_window) {
    int X, Y;
    Uint32 State = 0;
    p_resize = false;

    const Uint8* l_keyboard_state = SDL_GetKeyboardState( NULL );

    // Pull event
    while( SDL_PollEvent(&p_event) ) {
        // windows closed
        if (p_event.type == SDL_WINDOWEVENT) {
            switch (p_event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    p_resize = true;
                    p_resize_w = p_event.window.data1;
                    p_resize_h = p_event.window.data2;
                    //t_graphic->ResizeWindow( p_event.window.data1, p_event.window.data2);
                break;
            }
        }

        if(p_event.type == SDL_QUIT)
            return false;
    }

    // Maus eingabe greifen
    if( p_grab_mouse) { // Maus winkel erechnen
        State = SDL_GetMouseState( &X, &Y); // Rudi
        mappping.mouse_position.x = (float)X;
        mappping.mouse_position.y = (float)Y;
        mappping.mouse_position.x /= l_width;
        mappping.mouse_position.y /= l_height;
        mappping.mouse_position.x -= 0.5;
        mappping.mouse_position.y -= 0.5;
        if( fabs (mappping.mouse_position.x) < 0.0015f && fabs(mappping.mouse_position.y) < 0.0015f ) {// Rundungsfehler
            mappping.mouse_position.x = 0;
            mappping.mouse_position.y = 0;
        }
        SDL_WarpMouseInWindow( l_window, l_width/2, l_height/2); // Maus in die mitte ziehen
    } else {
        mappping.mouse_position.x = 0;
        mappping.mouse_position.y = 0;
    }
    // Mauskeys
    if( State & SDL_BUTTON(SDL_BUTTON_LEFT) || l_keyboard_state[SDL_SCANCODE_2]) {
        mappping.destory = true;
    }
    if( State & SDL_BUTTON(SDL_BUTTON_RIGHT) || l_keyboard_state[SDL_SCANCODE_1]) {
        mappping.place = true;
    }
    // special keys
    if( l_keyboard_state[ SDL_SCANCODE_F5])
        mappping.refresh = true;
    // Richtungstasten
    if( l_keyboard_state[SDL_SCANCODE_LEFT] | l_keyboard_state[SDL_SCANCODE_A] )
        mappping.left = true;
    if( l_keyboard_state[SDL_SCANCODE_RIGHT] | l_keyboard_state[SDL_SCANCODE_D] )
        mappping.right = true;
    if( l_keyboard_state[SDL_SCANCODE_UP] | l_keyboard_state[SDL_SCANCODE_W] )
        mappping.up = true;
    if( l_keyboard_state[SDL_SCANCODE_DOWN] | l_keyboard_state[SDL_SCANCODE_S] )
        mappping.down = true;
    // Sondertasten
    if( l_keyboard_state[SDL_SCANCODE_ESCAPE] )
        mappping.mouse_grab = true;
    if( l_keyboard_state[SDL_SCANCODE_SPACE] |l_keyboard_state[SDL_SCANCODE_R] )
        mappping.jump = true;
    if( l_keyboard_state[SDL_SCANCODE_F] )
        mappping.shift = true;
    if( l_keyboard_state[SDL_SCANCODE_E] || l_keyboard_state[SDL_SCANCODE_I] )
        mappping.inventory = true;

    // grabbing mouse
    if( mappping.mouse_grab && !mappping_previously.mouse_grab ) {
        p_grab_mouse = !p_grab_mouse;
        SDL_ShowCursor( !p_grab_mouse);
    }
    return true;
}

bool handle::process() {
    // pool the events
    while( SDL_PollEvent(&p_event) ) {
        // windows closed
        if (p_event.type == SDL_WINDOWEVENT) {
            switch (p_event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    p_resize = true;
                    p_resize_w = p_event.window.data1;
                    p_resize_h = p_event.window.data2;
                break;
            }
        }

        if( p_event.type == SDL_QUIT)
            return false;
    }

    const Uint8* l_keyboard_state = SDL_GetKeyboardState( NULL );
    if( l_keyboard_state[SDL_SCANCODE_Q] )
        return false;
    return true;
}
