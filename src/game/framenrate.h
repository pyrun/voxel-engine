#ifndef FRAMENRATE_H
#define FRAMENRATE_H

#include <SDL2\SDL.h>
#include <vector>

class Framenrate_List {
public:
    Framenrate_List() {
        m_max = 10;
    }
    void AddTime( int time) {
        timeTimer.push_back( time);
        if( (int)timeTimer.size() > m_max )
            timeTimer.erase( timeTimer.begin());
    }
    float GetTime() {
        int fps = 0;
        for( int i = 0; i < (int)timeTimer.size(); i++)
            fps += timeTimer[i];
        return (float)fps/(int)timeTimer.size();
    }
    void SetMax( int Max) {
        m_max = Max;
    }
protected:
private:
    std::vector<int> timeTimer;
    int m_max;
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
    double getFrameratePrecisely() { return m_fps; }
    double getMSframe() { return m_msframe; }
protected:
private:
    int m_start;
    int m_end;
    float m_rate;
    Framenrate_List List;

    int m_lastTime = SDL_GetTicks();
    int m_nbFrames = 0;
    double m_fps = 0.0;
    double m_msframe = 0.0;
};

#endif // FRAMENRATE_H
