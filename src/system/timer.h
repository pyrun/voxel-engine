#ifndef TIMER_H
#define TIMER_H

#include <SDL2\SDL.h>
#include <vector>

class Timer {
public:
    Timer();
    virtual ~Timer();

    void Start();
    bool IsRunnig();
    void Stop();
    void Pause();
    int GetTicks();

    void Set( int time) {
        TimeStart = SDL_GetTicks()-time;
    }
protected:
private:
    int TimeStart;
    int TimeEnd;
    int TimePause;
    bool Running;
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
