#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "world.h"
#include "input.h"

class player
{
    public:
        player( world *world);
        virtual ~player();

        void setPosition( glm::vec3 position);

        void input( Input *input, Camera *camera, int delta);

        glm::vec3 getPositonHead();
    protected:

    private:
        glm::vec3 p_position;

        int p_object_id;
        world *p_target_world;
};

#endif // PLAYER_H
