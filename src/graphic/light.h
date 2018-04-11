#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <vector>

class light
{
    public:
        light( int id);
        virtual ~light();

        void setPositon( glm::vec3 pos) { p_positions = pos; }
        void setColor( glm::vec3 color) { p_colors = color; }
        int getId() { return p_id; }
        glm::vec3 getPos() { return p_positions; }
        glm::vec3 getColor() { return p_colors; }
    protected:

    private:
        int p_id;
        glm::vec3 p_positions;
        glm::vec3 p_colors;
};

#endif // LIGHT_H
