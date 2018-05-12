#ifndef LANDSCAPE_GENERATOR_H
#define LANDSCAPE_GENERATOR_H

#include "block.h"
#include "chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

void Landscape_Generator( Chunk* c_chunk, block_list* b_list);

#endif // LANDSCAPE_GENERATOR_H
