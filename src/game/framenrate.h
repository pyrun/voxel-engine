#ifndef FRAMENRATE_H
#define FRAMENRATE_H

#include <SDL2/SDL.h>
#include <vector>

class Framenrate_List {
public:
    Framenrate_List() {
        p_max = 10;
    }
    void AddTime( int time) {
        timeTimer.push_back( time);
        if( (int)timeTimer.size() > p_max )
            timeTimer.erase( timeTimer.begin());
    }
    float GetTime() {
        int fps = 0;
        for( int i = 0; i < (int)timeTimer.size(); i++)
            fps += timeTimer[i];
        return (float)fps/(int)timeTimer.size();
    }
    void SetMax( int Max) {
        p_max = Max;
    }
protected:
private:
    std::vector<int> timeTimer;
    int p_max;
};

class Framenrate {
public:
    Framenrate();
    virtual ~Framenrate();

    void Set( int Hz);
    void StartCount();
    void EndCount();
    void CalcDelay();
    float getFramenrate();
    double getFrameratePrecisely() { return p_fps; }
    double getMSframe() { return p_msframe; }
    int getLimit() { return p_rate; }
protected:
private:
    int p_start;
    int p_end;
    float p_rate;
    Framenrate_List List;

    int p_lastTime = SDL_GetTicks();
    int p_nbFrames = 0;
    double p_fps = 0.0;
    double p_msframe = 0.0;
};

#endif // FRAMENRATE_H
