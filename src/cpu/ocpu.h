#ifndef OCPU_H
#define OCPU_H

#include <iostream>

#define OCPU_MAX_REGISTER 8
#define OCPU_RAM_SIZE 65536
#define OCPU_GRAPHIC_RAM_SIZE 256*256
#define int16 int16_t

enum ocpu_instruction_set
{
    LOAD = 0x0,
    ADD,
    SUB,
    MUL,
    DIV
};

class ocpu
{
    public:
        ocpu();
        virtual ~ocpu();

        void step();

        void reset();

    protected:
    private: // function
        //int16* operand(int16 code);
    private: // variable
        int16 p_memory[ OCPU_RAM_SIZE];
        int16 p_graphic_memory[ OCPU_GRAPHIC_RAM_SIZE]; // write only
        int16 p_register[ OCPU_MAX_REGISTER];
        int16 m_overflow;

        unsigned int p_programcounter_steps;
        unsigned int p_programcounter_cycles;
};

#endif // OCPU_H
