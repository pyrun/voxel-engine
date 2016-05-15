#include "timer.h"

Timer::Timer() {
    Running = false;
    TimeStart = 0;
    TimeEnd = 0;
}
Timer::~Timer() {
}
void Timer::Start() {
    TimeStart = SDL_GetTicks();
    Running = true;
}

bool Timer::IsRunnig() {
    return Running;
}

void Timer::Pause() {
    if( Running == true) {
        TimePause = SDL_GetTicks(); // timer zeit aufnhemn
        Running = false;
    } else {
        TimeStart += SDL_GetTicks()-TimePause; // Timer Pause abziehen
        Running = true; // Läuft ja wieder
    }
}
void Timer::Stop() { // Reset
    if( Running == false) {
        Pause();
        //SDL_Log( "Bevor ein Timer gestoppt werden soll sollte Pause entfert werden!");
    }
    TimeEnd = SDL_GetTicks(); //
}
int Timer::GetTicks() {
    return SDL_GetTicks() - TimeStart;
}

