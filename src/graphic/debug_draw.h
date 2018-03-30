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

        void draw( glm::mat4 viewmatrix, Shader *shader);

        void drawLine( glm::vec3 from, glm::vec3 to, glm::vec3 color);
        void drawCube( glm::vec3 from, glm::vec3 size, glm::vec3 color);
    private:
        bool p_change;

        int p_vector_length;
        int p_vector_size;

        std::vector<glm::vec3> p_lines;
        std::vector<glm::vec3> p_color;

        GLuint p_vbo_lines;
        GLuint p_vbo_color;
        GLuint p_vao;

        GLuint p_mvp;
};

#endif // DEBUG_DRAW_H
