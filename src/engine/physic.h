#ifndef PHYSIC_H
#define PHYSIC_H

#include <glm/glm.hpp>

#include "object.h"
#include "chunk.h"

namespace physic
{
    bool checkLineCollision( Chunk* chunk, glm::vec3 from, glm::vec3 to);

    glm::vec3 checkCollisionVoxel( Chunk* chunk, object *object);
};

#endif // PHYSIC_H
