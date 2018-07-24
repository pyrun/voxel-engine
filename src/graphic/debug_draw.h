#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "graphic.h"

class debug_draw {
    public:
        debug_draw();
        ~debug_draw();

        void cube( glm::vec3 positon, glm::vec3 size, glm::vec3 color);

        void clear();

        void draw( Shader *shader);
    private:
        bool p_change;

        glm::vec3 p_scale;

        std::vector<glm::vec3> p_vertex;
        std::vector<glm::vec3> p_normal;
        std::vector<glm::vec3> p_color;
        std::vector<unsigned int> p_indices;

        GLuint p_vbo_indices;
        GLuint p_vbo_vertex;
        GLuint p_vbo_normal;
        GLuint p_vbo_color;
        GLuint p_vao;
};

#endif // DEBUG_DRAW_H
