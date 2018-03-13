#include "input.h"

Input::Input() {
    GrabMouse = false;
    Reset(); // anfangswerte reseten
    Reset(); // 2te mal um auch altemap reset durchzuführen
}
Input::~Input() {

}
void Input::Reset() {
    MapOld = Map; // Merken als alt
    // richtungspfeile
    Map.Up = false;
    Map.Down = false;
    Map.Right = false;
    Map.Left = false;
    // Invetory
    Map.Inventory = false;
    Map.Jump = false;
    Map.Shift = false;

    Map.Place = false;
    Map.Destory = false;

    Map.MouseGrab = false;
}

bool Input::Handle( int l_width, int l_height, SDL_Window* l_window) {
    int X, Y;
    Uint32 State = NULL;
    p_resize = false;
    const Uint8* Keys = SDL_GetKeyboardState( NULL );
    // Pull all keys
    while( SDL_PollEvent(&p_event) ) {
        // Windows event auffangen
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
    if( GrabMouse) { // Maus winkel erechnen
        State = SDL_GetMouseState( &X, &Y); // Rudi
        Map.MousePos.x = (float)X;
        Map.MousePos.y = (float)Y;
        Map.MousePos.x /= l_width;
        Map.MousePos.y /= l_height;
        Map.MousePos.x -= 0.5;
        Map.MousePos.y -= 0.5;
        if( fabs (Map.MousePos.x) < 0.001 && fabs(Map.MousePos.y) < 0.001 ) {// Rundungsfehler
            Map.MousePos.x = 0;
            Map.MousePos.y = 0;
        }
        SDL_WarpMouseInWindow( l_window, l_width/2, l_height/2); // Maus in die mitte ziehen
    } else {
        Map.MousePos.x = 0;
        Map.MousePos.y = 0;
    }
    // Mauskeys
    if( State & SDL_BUTTON(SDL_BUTTON_LEFT) || Keys[SDL_SCANCODE_2]) {
        Map.Destory = true;
    }
    if( State & SDL_BUTTON(SDL_BUTTON_RIGHT) || Keys[SDL_SCANCODE_1]) {
        Map.Place = true;
    }
    // Richtungstasten
    if( Keys[SDL_SCANCODE_LEFT] | Keys[SDL_SCANCODE_A] )
        Map.Left = true;
    if( Keys[SDL_SCANCODE_RIGHT] | Keys[SDL_SCANCODE_D] )
        Map.Right = true;
    if( Keys[SDL_SCANCODE_UP] | Keys[SDL_SCANCODE_W] )
        Map.Up = true;
    if( Keys[SDL_SCANCODE_DOWN] | Keys[SDL_SCANCODE_S] )
        Map.Down = true;
    // Sondertasten
    if( Keys[SDL_SCANCODE_ESCAPE] )
        Map.MouseGrab = true;
    if( Keys[SDL_SCANCODE_SPACE] |Keys[SDL_SCANCODE_R] )
        Map.Jump = true;
    if( Keys[SDL_SCANCODE_F] )
        Map.Shift = true;
    if( Keys[SDL_SCANCODE_E] || Keys[SDL_SCANCODE_I] )
        Map.Inventory = true;
    if( Map.MouseGrab && !MapOld.MouseGrab ) {
        GrabMouse = !GrabMouse;
        SDL_ShowCursor( !GrabMouse);
    }
    return true;
}
