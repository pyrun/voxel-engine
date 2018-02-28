#include "object.h"

using namespace tinyxml2;

bool file_exist( std::string file) {
    if( access( file.c_str(), F_OK ) != -1 )
        return true;
    return false;
}

object_type::object_type()
{
    // Buffer erzeugen
    glGenVertexArrays(1, &p_vao );
    glGenBuffers(1, &p_vbo_vertices);
    glGenBuffers(1, &p_vbo_normal);
    glGenBuffers(1, &p_vbo_texture);
    glGenBuffers(1, &p_vbo_index);
}

object_type::~object_type()
{
    glDeleteVertexArrays(1, &p_vao );
    glDeleteBuffers(1, &p_vbo_vertices);
    glDeleteBuffers(1, &p_vbo_normal);
    glDeleteBuffers(1, &p_vbo_texture);
    glDeleteBuffers(1, &p_vbo_index);
}

bool object_type::load_type( config *config, std::string l_path, std::string l_name) {
    XMLDocument l_file;
    bool l_idle = false;

    l_path = l_path + l_name + "/";

    std::string l_pathfile = l_path + (char*)DEFINITION_FILE;

    // if file dont exist - dont load
    if( file_exist( l_pathfile) == false) {
        return false;
    }

    // load the file
    XMLError l_result = l_file.LoadFile( l_pathfile.c_str());

    // check the file
    XMLCheckResult(l_result);

    std::string l_object_name;

    //
    XMLElement* l_object = l_file.FirstChildElement( "object" );
    if( l_object == nullptr)
        return false;

    if( l_object->Attribute( "name"))
        l_object_name = l_object->Attribute( "name");


    printf( "l_object_name %s\n", l_object_name.c_str());
}

void object_type::updateVbo()
{
    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertices);
    glBufferData( GL_ARRAY_BUFFER, p_vertices.size() * sizeof( glm::vec3 ), &p_vertices[0], GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
    glBufferData( GL_ARRAY_BUFFER, p_normal.size() * sizeof( glm::vec3 ), &p_normal[0], GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_texture);
    glBufferData( GL_ARRAY_BUFFER, p_texcoords.size() * sizeof( glm::vec2 ), &p_texcoords[0], GL_STATIC_DRAW);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vbo_index);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof( unsigned int ), &p_indices[0], GL_STATIC_DRAW);
    glBindBuffer( GL_ARRAY_BUFFER, 0);
}

void object_type::updateVao()
{
    glBindVertexArray( p_vao);

    glEnableVertexAttribArray( 0);  // Vertex position
    glEnableVertexAttribArray( 1);  // Vertex normal
    glEnableVertexAttribArray( 2);  // Vertex color

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vbo_index);

    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertices);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
    glBindBuffer( GL_ARRAY_BUFFER, p_vbo_texture);
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

    glBindVertexArray(0);
}

void object_type::draw( Transform* transform, Shader* shader, glm::mat4 viewprojection)
{
    p_texture->Bind();

    // draw
    glBindVertexArray( p_vao);

    shader->update( transform, viewprojection);
    glDrawElements( GL_TRIANGLES, p_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

object::object()
{
    p_type = NULL;
}

object::~object() {

}

void object::init()
{

}

void object::draw( Shader* shader, glm::mat4 viewprojection) {
    if( p_type)
        p_type->draw( getTransform(), shader, viewprojection);
}

object_handle::object_handle()
{

}

object_handle::~object_handle()
{
    for( auto l_type:p_types)
        delete l_type;
}

bool object_handle::load( config *config) {
    std::string l_path;

    l_path = config->get( "folder", "objects", "objects/");

    load_folder( l_path, config);

    return true;
}

bool object_handle::load_folder( std::string folder, config *config) {
    DIR *l_dir;

    struct dirent *l_entry;

    // open dir
    l_dir = opendir(folder.c_str());
    if ( l_dir == NULL)  // error opening the directory?
        return false;

    // open ech folder
    while ((l_entry = readdir(l_dir)) != NULL) {
        std::string l_file = folder + l_entry->d_name + "/";

        // dont go back folder
        if( l_entry->d_name[0] == '.')
            continue;

        object_type *l_type = new object_type();

        // load type
        if( l_type->load_type( config, folder, l_entry->d_name) == false) {
            delete l_type;
            load_folder( l_file, config);
        }
        p_types.push_back( l_type);
    }
    closedir(l_dir);

    return true;
}
