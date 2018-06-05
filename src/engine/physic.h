#ifndef PHYSIC_H
#define PHYSIC_H

#include <glm/glm.hpp>

namespace physic
{
    enum hit_side {
        ground = 0,
        top,
        west,
        east,
        north,
        south
    };

    class hit{
        public:
            hit();
            ~hit();

            void set( hit_side side, bool value);
            bool get( hit_side side);

            void reset();
        public:
            bool ground;
            bool top;
            bool west;
            bool east;
            bool north;
            bool south;
    };

    bool testAABB( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2);
    bool testAABB2( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2);
};

#endif // PHYSIC_H
