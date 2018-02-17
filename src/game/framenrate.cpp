#include "framenrate.h"
#include <stdio.h>

Framenrate::Framenrate() {
    p_start = 0;
    p_end = 0;
    p_rate = 16;
    List.SetMax( 10);
    p_lastTime = 0;
}

Framenrate::~Framenrate() {

}

void Framenrate::Set( int Hz) {
    p_rate = 1.0f/Hz*1000; // 1/f -> sec benötigen aber ms -> *1000
}

void Framenrate::StartCount() {
    p_start = SDL_GetTicks();
}

void Framenrate::EndCount() {
    p_end = SDL_GetTicks();
}

void Framenrate::CalcDelay() {
    int p_currentTime = SDL_GetTicks();
    p_nbFrames++;
    if ( p_currentTime - p_lastTime >= 1000 ){ // If last prinf() was more than 1 sec ago
         // printf and reset timer
         //printf("%f ms/frame %f\n", 1000.0/double(nbFrames), 1.0f/(1000.0/double(nbFrames))*1000 );
         p_fps = 1.0/(1000.f/double( p_nbFrames))*1000;
         p_msframe = 1000.f/ double( p_nbFrames);
         p_nbFrames = 0;

         p_lastTime += 1000;
        }
    // Falls counter kein ende hat einen setzten
    if( p_end == 0)
        EndCount();
    // Zeit ausrechnen
    int p_delay = p_end - p_start;
    // Zeit zu hoch
    if( p_delay > p_rate)
        return;
    SDL_Delay( (int)p_rate - p_delay); // Abwarten
    // delay abziehen
    float t_fps = (float)1000.0f/( SDL_GetTicks() - p_start);
    List.AddTime( (int)t_fps);
    // Reset
    p_start = 0;
    p_end = 0;
}

float Framenrate::getFramenrate() {
    return List.GetTime();
}
