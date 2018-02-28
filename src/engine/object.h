#ifndef OBJECT_H
#define OBJECT_H

#include <dirent.h>
#include <algorithm>

#include "../xml/tinyxml2.h"

#include "../graphic/graphic.h"
#include "../graphic/tiny_obj_loader.h"
#include "../graphic/texture.h"

#define DEFINITION_FILE "definition.xml"

class object_type {
    public:
        object_type();
        ~object_type();

        bool load_type( config *config, std::string l_path, std::string l_name);

        void updateVbo();
        void updateVao();

        void draw( Transform* transform, Shader* shader, glm::mat4 viewprojection);

    private:
        std::string l_name;
        texture *p_texture;

        std::vector<glm::vec3> p_vertices;
        std::vector< glm::vec2 > p_texcoords;
        std::vector<glm::vec3> p_normal;
        std::vector<unsigned int> p_indices;

        GLuint p_vao;
        GLuint p_vbo_vertices;
        GLuint p_vbo_normal;
        GLuint p_vbo_index;
        GLuint p_vbo_texture;
};

class object {
    public:
        object();
        ~object();

        void init();
        void draw( Shader* shader, glm::mat4 viewprojection);

        Transform *getTransform() { return &p_transform; }
    protected:
    private:
        Transform p_transform;

        object_type *p_type;
};

class object_handle
{
    public:
        object_handle();
        ~object_handle();

        bool load( config *config);
        bool load_folder( std::string folder, config *config);
    private:

        std::vector<object_type*> p_types;

};

#endif // OBJECT_H
