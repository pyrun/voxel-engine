#ifndef PHYSIC_H
#define PHYSIC_H

#include <glm/glm.hpp>

#include "object.h"
#include "chunk.h"

namespace physic
{
    bool testAABB( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2);

    bool checkLineCollision( Chunk* chunk, glm::vec3 from, glm::vec3 to);

    glm::vec3 checkCollisionVoxel( Chunk* chunk, object *object);
};

#endif // PHYSIC_H
