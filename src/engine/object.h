#ifndef OBJECT_H
#define OBJECT_H

#include <dirent.h>
#include <algorithm>

#include "../xml/tinyxml2.h"

#include "../graphic/graphic.h"
#include "../graphic/texture.h"
#include "../graphic/debug_draw.h"

#define DEFINITION_FILE "definition.xml"

class object_type {
    public:
        object_type();
        ~object_type();

        void init( Transform *transform);

        char *load_type( config *config, std::string l_path, std::string l_name);
        bool load_file( std::string file);

        void updateVbo();
        void updateVao();

        void draw( glm::mat4 model, Shader* shader);
        void draw_debug( glm::mat4 model, Shader* shader);

        std::string getName() { return p_name; }
        glm::vec3 getScale() { return p_size; }

        glm::vec3 getHitbox() { return p_hitbox_size; }
        glm::vec3 getHitboxOffset() { return p_hitbox_offset; }
    private:
        std::string p_name;
        std::string p_file;
        std::string p_texture_file;
        texture *p_texture;

        std::vector<glm::vec3> p_vertices;
        std::vector< glm::vec2 > p_texcoords;
        std::vector<glm::vec3> p_normal;
        std::vector<unsigned int> p_indices;
        bool p_model_changed;

        glm::vec3 p_size;

        GLuint p_vao;
        GLuint p_vbo_vertices;
        GLuint p_vbo_normal;
        GLuint p_vbo_index;
        GLuint p_vbo_texture;

        glm::vec3 p_hitbox_offset;
        glm::vec3 p_hitbox_size;

        debug_draw p_debug_draw;
};

class object {
    public:
        object();
        ~object();

        void init();

        void process();
        void process_phyisc();

        void draw( Shader* shader);
        void draw_debug( Shader* shader);

        void update_model();

        void setType( object_type *type);
        object_type *getType() { return p_type; }

        void setId( int id) { p_id = id; }
        int getId() { return p_id; }

        void addVelocity( glm::vec3 velocity) { p_velocity += velocity; }
        void setVelocity( glm::vec3 velocity) { p_velocity = velocity; }
        glm::vec3 getVerlocity() { return p_velocity; }

        void setPosition( glm::vec3 position);
        void addPosition( glm::vec3 position) { p_position += position; }
        glm::vec3 getPosition() { return p_position; }

        void setRotation( glm::vec3 rotation);
        void addRotation( glm::vec3 rotation) { p_rotation += rotation; }

        void setUpdate( bool change = true) { p_model_change = change; }
    protected:
    private:
        int p_id;

        bool p_model_change;

        glm::mat4 p_model;
        glm::vec3 p_position;
        glm::vec3 p_rotation;
        glm::vec3 p_scale;

        object_type *p_type;

        glm::vec3 p_velocity;
};

class object_handle
{
    public:
        object_handle();
        ~object_handle();

        bool load( config *config);
        bool load_folder( std::string folder, config *config);

        object_type *get( std::string name);
    private:

        std::vector<object_type *> p_types;

};

#endif // OBJECT_H
