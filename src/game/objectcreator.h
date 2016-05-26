#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include <vector>
#include <glm/glm.hpp>

class ObjectCreator {
    public:
        ObjectCreator();
        virtual ~ObjectCreator();

        std::vector<glm::vec3> getVertices() { return m_vertices;}
        std::vector<unsigned int> getIndices() { return m_indices;}
        std::vector<glm::vec2> getTexcoords() { return m_texcoords;}

        void addIndices( int l_vertices_end, bool l_flip = false);

        void addCube( glm::vec3 l_pos, glm::vec3 l_size, glm::vec4 l_texture);

        void addXPlate( glm::vec3 l_pos = glm::vec3( 0, 0, 0), glm::vec3 l_size = glm::vec3( 1, 1, 1), glm::vec4 l_texture = glm::vec4( 0, 0, 1, 1), bool flip = false);
        void addYPlate( glm::vec3 l_pos = glm::vec3( 0, 0, 0), glm::vec3 l_size = glm::vec3( 1, 1, 1), glm::vec4 l_texture = glm::vec4( 0, 0, 1, 1), bool flip = false);
        void addZPlate( glm::vec3 l_pos = glm::vec3( 0, 0, 0), glm::vec3 l_size = glm::vec3( 1, 1, 1), glm::vec4 l_texture = glm::vec4( 0, 0, 1, 1), bool flip = false);
    protected:
    private:
        std::vector<glm::vec3> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector< glm::vec2 > m_texcoords;


};

#endif // OBJECTCREATOR_H
