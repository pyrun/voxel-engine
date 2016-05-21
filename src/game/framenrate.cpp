#include "framenrate.h"
#include <stdio.h>

Framenrate::Framenrate() {
    m_start = 0;
    m_end = 0;
    m_rate = 16;
    List.SetMax( 10);
    m_lastTime = 0;
}

Framenrate::~Framenrate() {

}

void Framenrate::Set( int Hz) {
    m_rate = 1.0f/Hz*1000; // 1/f -> sec benötigen aber ms -> *1000
}

void Framenrate::StartCount() {
    m_start = SDL_GetTicks();
}

void Framenrate::EndCount() {
    m_end = SDL_GetTicks();
}

void Framenrate::CalcDelay() {
    int m_currentTime = SDL_GetTicks();
    m_nbFrames++;
    if ( m_currentTime - m_lastTime >= 1000 ){ // If last prinf() was more than 1 sec ago
         // printf and reset timer
         //printf("%f ms/frame %f\n", 1000.0/double(nbFrames), 1.0f/(1000.0/double(nbFrames))*1000 );
         m_fps = 1.0/(1000.f/double( m_nbFrames))*1000;
         m_msframe = 1000.f/ double( m_nbFrames);
         m_nbFrames = 0;

         m_lastTime += 1000;
        }
    // Falls counter kein ende hat einen setzten
    if( m_end == 0)
        EndCount();
    // Zeit ausrechnen
    int m_delay = m_end - m_start;
    // Zeit zu hoch
    if( m_delay > m_rate)
        return;
    SDL_Delay( (int)m_rate - m_delay); // Abwarten
    // delay abziehen
    float t_fps = (float)1000.0f/( SDL_GetTicks() - m_start);
    List.AddTime( (int)t_fps);
    // Reset
    m_start = 0;
    m_end = 0;
}

float Framenrate::getFramenrate() {
    return List.GetTime();
}
