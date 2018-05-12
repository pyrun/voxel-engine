#ifndef LANDSCAPE_GENERATOR_H
#define LANDSCAPE_GENERATOR_H

#include "block.h"
#include "chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

std::vector<glm::ivec3> Landscape_Generator( Chunk* chunk, block_list* list);

#endif // LANDSCAPE_GENERATOR_H
