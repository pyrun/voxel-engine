#include "ocpu.h"

ocpu::ocpu()
{
    // reset
    reset();
}

ocpu::~ocpu()
{
    //dtor
}

void ocpu::step() {
    p_programcounter_steps++;
    if( p_programcounter_cycles < p_programcounter_steps) {
        //p_programcounter_cycles;
    }
}

void ocpu::reset() {
    p_programcounter_cycles = 0;
    p_programcounter_steps = 0;

    m_overflow = 0;

    for( int i = 0; i < OCPU_MAX_REGISTER; i++)
        p_register[i] = 0x00;

    for( int i = 0; i < OCPU_RAM_SIZE; i++)
        p_memory[i] = 0x00;

    for( int i = 0; i < OCPU_GRAPHIC_RAM_SIZE; i++)
        p_graphic_memory[i] = 0x00;
}
