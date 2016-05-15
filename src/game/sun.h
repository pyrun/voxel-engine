#ifndef SUN_H
#define SUN_H

#include "../graphic/graphic.h"
#include "../system/timer.h"

#define TIME_BORDER 60*1000//1*60*1000

class Sun {
public:
    Sun();
    virtual ~Sun();
    void Process( Shader *t_shader, Graphic *t_graphic) {
        // Shader Sonne
        t_shader->Bind();
        t_shader->SetBackgroundcolor( m_red, m_green, m_blue, 1);

        t_shader->SetSun( m_red, m_green, m_blue, 1);

        // Hintergrundfarbe setzten
        t_graphic->GetDisplay()->SetBackgroundColor( m_red, m_green, m_blue, 1);

        double pi = acos(-1);

        m_red = cosf( pi / ((float)TIME_BORDER/m_time.GetTicks()) - (pi/2) );
        // http://www.abi-physik.de/buch/das-elektrische-feld/kondensator/
        m_green = m_red;
        m_blue = m_red;

        // Time
        if( m_time.GetTicks() > TIME_BORDER) {
            m_time.Start();
        }
        //printf( "%f\n", m_red);
    }

    void SetDay() { m_time.Set( TIME_BORDER/2 ); }
protected:
private:
    Timer m_time;
    float m_red;
    float m_green;
    float m_blue;

};

#endif // SUN_H
