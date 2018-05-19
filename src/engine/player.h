#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

class player
{
    public:
        player();
        virtual ~player();

        void setPosition( glm::vec3 position);
    protected:

    private:
        glm::vec3 p_position;

        object *p_object;
};

#endif // PLAYER_H
