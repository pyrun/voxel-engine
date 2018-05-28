#include "physic.h"

const float SMALLER = (1.0f / 16.0f); // 16pxl width
const float EPSILON = (1.0f / 1024.0f);

bool physic::testAABB( glm::vec3 position1, glm::vec3 size1, glm::vec3 position2, glm::vec3 size2)
{
   //check the X axis
   /*if( std::fabs( position1.x - position2.x) < size1.x + size2.x)
   {
      //check the Y axis
      if( std::fabs( position1.y - position2.y) < size1.y + size2.y)
      {
          //check the Z axis
          if( std::fabs( position1.z - position2.z) < size1.z + size2.z)
          {
             return true;
          }
      }
   }*/

   //return false;

    // check for overlap
    if ( std::fabs(position1.x - position2.x) > (size1.x + size2.x) ) return false;
    if ( std::fabs(position1.y - position2.y) > (size1.y + size2.y) ) return false;
    if ( std::fabs(position1.z - position2.z) > (size1.z + size2.z) ) return false;
    return true;
};

bool physic::checkLineCollision( Chunk* chunk, glm::vec3 from, glm::vec3 to) {

    bool l_found = false;
    glm::ivec3 l_from = glm::ivec3( floor( from.x), floor( from.y), floor( from.z));
    glm::ivec3 l_to = glm::ivec3( floor( to.x), floor( to.y), floor( to.z));

    int sx = l_to.x > l_from.x ? 1 : l_to.x < l_from.x ? -1 : 0;
    int sy = l_to.y > l_from.y ? 1 : l_to.y < l_from.y ? -1 : 0;
    int sz = l_to.z > l_from.z ? 1 : l_to.z < l_from.z ? -1 : 0;

    int gx = l_from.x;
    int gy = l_from.y;
    int gz = l_from.z;

    //Planes for each axis that we will next cross
    int gxp = l_from.x + (l_to.x > l_from.x ? 1 : 0);
    int gyp = l_from.y + (l_to.y > l_from.y ? 1 : 0);
    int gzp = l_from.z + (l_to.z > l_from.z ? 1 : 0);

    //Only used for multiplying up the error margins
    int vx = to.x == from.x ? 1 : to.x - from.x;
    int vy = to.y == from.y ? 1 : to.y - from.y;
    int vz = to.z == from.z ? 1 : to.z - from.z;

    //Error is normalized to vx * vy * vz so we only have to multiply up
    int vxvy = vx * vy;
    int vxvz = vx * vz;
    int vyvz = vy * vz;

    //Error from the next plane accumulators, scaled up by vx*vy*vz
    // gx0 + vx * rx === gxp
    // vx * rx === gxp - gx0
    // rx === (gxp - gx0) / vx
    int errx = (gxp - from.x) * vyvz;
    int erry = (gyp - from.y) * vxvz;
    int errz = (gzp - from.z) * vxvy;

    int derrx = sx * vyvz;
    int derry = sy * vxvz;
    int derrz = sz * vxvy;

    do {
        if( chunk->getTile( glm::vec3( gx, gy, gz) ) != EMPTY_BLOCK_ID )
            l_found = true;

        if (gx == l_to.x && gy == l_to.y && gz == l_to.z) break;

        //Which plane do we cross first?
        int xr = abs(errx);
        int yr = abs(erry);
        int zr = abs(errz);

        if (sx != 0 && (sy == 0 || xr < yr) && (sz == 0 || xr < zr)) {
            gx += sx;
            errx += derrx;
        }
        else if (sy != 0 && (sz == 0 || yr < zr)) {
            gy += sy;
            erry += derry;
        }
        else if (sz != 0) {
            gz += sz;
            errz += derrz;
        }

    } while (true);
    //printf( "test\n");

    return l_found;
}

glm::vec3 physic::checkCollisionVoxel( Chunk* chunk, object *object) {
    int l_position_start;
    int l_position_end;

    glm::vec3 l_position = object->getPosition();
    glm::vec3 l_velocity = object->getVerlocity();
    glm::vec3 collision_size = glm::vec3( 1, 2, 1);

    if( l_velocity.x != 0) {
        int l_position_min_x = truncf( l_position.x + SMALLER);
        int l_position_max_x = truncf( l_position.x + collision_size.x - SMALLER);
        int l_position_min_y = truncf( l_position.y + SMALLER);
        int l_position_max_y = truncf( l_position.y + collision_size.y - SMALLER);
        int l_position_min_z = truncf( l_position.z + SMALLER);
        int l_position_max_z = truncf( l_position.z + collision_size.z - SMALLER);

        if( l_velocity.x > 0){
            l_position_start = l_position_max_x;
            l_position_end = truncf(l_position.x + collision_size.x + l_velocity.x - SMALLER);
        } else {
            l_position_start = l_position_min_x;
            l_position_end = truncf(l_position.x + l_velocity.x + SMALLER);
        }

        for( int l_pos_x = l_position_start; l_pos_x < l_position_end; l_pos_x++ ) {

            if( checkLineCollision( chunk, glm::vec3( l_pos_x, l_position_min_y, l_position_min_z), glm::vec3(l_pos_x, l_position_max_y, l_position_max_z))){
                // align to surface (EPSILON pushes the character slightly away from the wall)
                if( l_velocity.x > 0){
                    l_position.x = l_pos_x - collision_size.x + SMALLER - EPSILON;
                } else {
                    l_position.x = l_pos_x - SMALLER + EPSILON;
                }
                l_velocity.x = 0;
                break;
            }
        }
        l_position.x += l_velocity.x;
    }

    if( l_velocity.y != 0) {
        int l_position_min_x = truncf( l_position.x + SMALLER);
        int l_position_max_x = truncf( l_position.x + collision_size.x - SMALLER);
        int l_position_min_y = truncf( l_position.y + SMALLER);
        int l_position_max_y = truncf( l_position.y + collision_size.y - SMALLER);
        int l_position_min_z = truncf( l_position.z + SMALLER);
        int l_position_max_z = truncf( l_position.z + collision_size.z - SMALLER);

        if( l_velocity.y > 0){
            l_position_start = l_position_max_y;
            l_position_end = truncf(l_position.y + collision_size.y + l_velocity.y - SMALLER);
        } else {
            l_position_start = l_position_min_y;
            l_position_end = truncf(l_position.y + l_velocity.y + SMALLER);
        }

        for( int l_pos_y = l_position_start; l_pos_y < l_position_end; l_pos_y++ ) {

            if( checkLineCollision( chunk, glm::vec3( l_position_min_x, l_pos_y, l_position_min_z), glm::vec3( l_position_min_x, l_pos_y, l_position_max_z))){
                // align to surface (EPSILON pushes the character slightly away from the wall)
                if( l_velocity.y > 0){
                    l_position.y = l_pos_y - collision_size.y + SMALLER - EPSILON;
                } else {
                    l_position.y = l_pos_y - SMALLER + EPSILON;
                }
                l_velocity.y = 0;
                break;
            }
        }
        l_position.y += l_velocity.y;
    }
    l_velocity.y = 0;

    //object->setPosition( l_position );
    object->setVelocity( l_velocity );
}
