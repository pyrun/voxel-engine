#include "landscape_generator.h"

float simplex_noise(int octaves, glm::vec3 pos){
    float value = 0.0;
    int i;
    for(i=0; i<octaves; i++){
        value += glm::perlin( glm::vec3(
            pos.x*pow(2, i),
            pos.y*pow(2, i),
            pos.z*pow(2, i))
        );
    }
    return value;
}

float distance( glm::vec3 pos1, glm::vec3 pos2)
{
    float dx = abs( pos2.x - pos1.x);
    float dz = abs( pos2.z - pos1.z);

    return sqrt( dx*dx + dz*dz );
}

void Landscape_Generator( Chunk* chunk, block_list* b_list) {

    int l_stone = b_list->getByName( "stone")->getID();
    int l_grass = b_list->getByName( "grass")->getID();
    int l_earth = b_list->getByName( "earth")->getID();

    glm::ivec3 l_block;
    glm::vec3 l_real_pos;

    float l_size_inv = 30.f;

    glm::ivec3 l_position_chunk = chunk->getPos()*glm::ivec3(CHUNK_SIZE);

    for( l_block.x = 0; l_block.x < CHUNK_SIZE; l_block.x++) {

        for( l_block.z = 0; l_block.z < CHUNK_SIZE; l_block.z++) {

            float l_dstance_island = distance( glm::vec3(), l_position_chunk + l_block);

            int l_last_type;
            for( l_block.y = 0; l_block.y < CHUNK_SIZE; l_block.y++) {

                l_real_pos = l_position_chunk + l_block;

                float l_noise_top = (- ( glm::perlin( glm::vec2( l_real_pos.x, l_real_pos.z)/l_size_inv ) ))*10.f;
                float l_noise_down = (glm::perlin( glm::vec2( l_real_pos.x, l_real_pos.z)/l_size_inv ))*40.f;


                if(  l_dstance_island > 20.f) {
                    l_noise_top-= (l_dstance_island-20.f)/3.f;
                    l_noise_down+= (l_dstance_island-20.f)/3.f;
                }

                if( l_noise_top < l_real_pos.y)
                    continue;

                if( l_noise_down > l_real_pos.y)
                    continue;

                int l_type = l_stone;

                if( !(l_noise_top > l_real_pos.y+1) )
                    l_type = l_grass;
                else if( !(l_noise_top > l_real_pos.y+3) )
                    l_type = l_earth;

                l_last_type = l_type;
                chunk->set( l_block, l_type, false);
                //if( l_block.y != CHUNK_SIZE-1 && chunk->getTile( l_block - glm::ivec3( 0, -1, 0)) == EMPTY_BLOCK_ID)
                //    chunk->setTorchlight( l_block - glm::ivec3( 0, -1, 0), LIGHTING_MAX);
            }
        }
    }
}
