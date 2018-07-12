#include "timer.h"

timer::timer() {
    p_running = false;
    p_start = 0;
    p_end = 0;
}
timer::~timer() {
}
void timer::start() {
    p_start = SDL_GetTicks();
    p_running = true;
}

bool timer::isRunnig() {
    return p_running;
}

void timer::pause() {
    if( p_running == true) {
        p_pause = SDL_GetTicks(); // timer zeit aufnhemn
        p_running = false;
    } else {
        p_start += SDL_GetTicks() - p_pause; // Timer Pause abziehen
        p_running = true; // Läuft ja wieder
    }
}
void timer::stop() { // Reset
    if( p_running == false) {
        pause();
        //SDL_Log( "Bevor ein Timer gestoppt werden soll sollte Pause entfert werden!");
    }
    p_end = SDL_GetTicks(); //
}
int timer::getTicks() {
    return SDL_GetTicks() - p_start;
}

