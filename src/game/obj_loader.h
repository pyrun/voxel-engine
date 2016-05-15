#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

bool loadOBJ( const char * path, std::vector < glm::vec3 > & out_vertices, std::vector < glm::vec2 > & out_uvs, std::vector < glm::vec3 > & out_normals );

#endif // OBJ_LOADER_H
