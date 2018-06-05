#include "physic.h"

using namespace physic;

hit::hit() {
    reset();
}

hit::~hit() {

}

void hit::set( hit_side side, bool value) {
    switch( side) {
        case hit_side::ground: this->ground = value; break;
        case hit_side::top: this->top = value; break;
        case hit_side::west: this->west = value; break;
        case hit_side::south: this->south = value; break;
        case hit_side::north: this->north = value; break;
        case hit_side::east: this->east = value; break;
    }
}

bool hit::get( hit_side side) {
    switch( side) {
        case hit_side::ground:  return this->ground; break;
        case hit_side::top:     return this->top; break;
        case hit_side::west:    return this->west; break;
        case hit_side::south:   return this->south; break;
        case hit_side::north:   return this->north; break;
        case hit_side::east:    return this->east; break;
    }
    return false;
}

void hit::reset() {
    ground = false;
    top = false;
    west = false;
    south = false;
    north = false;
    east = false;
}

bool physic::testAABB( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2)
{
    // check for overlap
    if ( std::fabs(position1.x - position2.x) > (size1.x + size2.x) ) return false;
    if ( std::fabs(position1.y - position2.y) > (size1.y + size2.y) ) return false;
    if ( std::fabs(position1.z - position2.z) > (size1.z + size2.z) ) return false;
    return true;
};

bool physic::testAABB2( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2)
{
    // Collision x-axis?
    bool collisionX = position1.x + size1.x >= position2.x &&
        position2.x + size2.x >= position1.x;
    // Collision y-axis?
    bool collisionY = position1.y + size1.y >= position2.y &&
        position2.y + size2.y >= position1.y;
    // Collision z-axis?
    bool collisionZ = position1.z + size1.z >= position2.z &&
        position2.z + size2.z >= position1.z;
    // Collision only if on both axes
    return collisionX && collisionY && collisionZ;
}
