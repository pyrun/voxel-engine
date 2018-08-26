#ifndef ENTITY_DRAW_H
#define ENTITY_DRAW_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "graphic.h"

struct entity_draw_index {
    int vertex_id;
    int indices_id;
};

class entity_draw
{
    public:
        entity_draw();
        ~entity_draw();

        void cube( glm::vec3 positon, glm::vec3 size, glm::vec3 color);

        void clear();
        void setChange();

        void enable() { p_enable = true; }
        void disable() { p_enable = false; }

        void draw( Shader *shader);
    private:
        bool p_change;

        bool p_enable;

        glm::vec3 p_scale;

        std::vector<entity_draw_index> p_index;
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

#endif // ENTITY_DRAW_H
