#ifndef SUN_H
#define SUN_H

#include "../graphic/graphic.h"
#include "../system/timer.h"

#define TIME_BORDER 60*1000//1*60*1000

class Sun {
public:
    Sun();
    virtual ~Sun();
    void Process( Shader *t_shader, graphic *t_graphic) {
        // Shader Sonne
        t_shader->Bind();
        t_shader->SetBackgroundcolor( p_red, p_green, p_blue, 1);

        t_shader->SetSun( p_red, p_green, p_blue, 1);

        // Hintergrundfarbe setzten
        t_graphic->getDisplay()->setBackgroundColor( p_red, p_green, p_blue, 1);

        double pi = acos(-1);

        p_red = cosf( pi / ((float)TIME_BORDER/p_time.GetTicks()) - (pi/2) );
        // http://www.abi-physik.de/buch/das-elektrische-feld/kondensator/
        p_green = p_red;
        p_blue = p_red;

        // Time
        if( p_time.GetTicks() > TIME_BORDER) {
            p_time.Start();
        }
        //printf( "%f\n", p_red);
    }

    void SetDay() { p_time.Set( TIME_BORDER/2 ); }
protected:
private:
    Timer p_time;
    float p_red;
    float p_green;
    float p_blue;

};

#endif // SUN_H
