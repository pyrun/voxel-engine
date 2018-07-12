#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>
#include <vector>

class timer {
public:
    timer();
    virtual ~timer();

    void start();
    bool isRunnig();
    void stop();
    void pause();
    int getTicks();

    void set( int time) {
        p_start = SDL_GetTicks()-time;
    }
protected:
private:
    int p_start;
    int p_end;
    int p_pause;
    bool p_running;
};

class TimerList {
public:
    void AddTime( int time) {
        timeTimer.push_back( time);
        if( timeTimer.size() > 100 )
            timeTimer.erase( timeTimer.begin());
    }
    float GetTime() {
        int fps = 0;
        for( int i = 0; i < (int)timeTimer.size(); i++)
            fps += timeTimer[i];
        return (float)fps/(int)timeTimer.size();
    }
protected:
private:
    std::vector<int> timeTimer;
};

#endif // TIMER_H
