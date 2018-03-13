#ifndef LANDSCAPE_GENERATOR_H
#define LANDSCAPE_GENERATOR_H

#include "block.h"
#include "chunk.h"

float scaled_raw_noise_2d( const float loBound, const float hiBound, const float x, const float y );

void Landscape_Generator( Chunk* c_chunk, block_list* b_list);

#endif // LANDSCAPE_GENERATOR_H
