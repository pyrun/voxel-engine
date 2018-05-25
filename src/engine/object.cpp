#include "object.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../graphic/tiny_obj_loader.h"

#include <fstream>

using namespace tinyxml2;

bool file_exist( std::string file) {
    if( access( file.c_str(), F_OK ) != -1 )
        return true;
    return false;
}

object_type::object_type()
{
    p_texture = NULL;
    p_model_changed = false;
    p_vao = NULL;
}

object_type::~object_type()
{
    if( p_vao) {
        glDeleteVertexArrays(1, &p_vao);
        glDeleteBuffers(1, &p_vbo_vertices);
        glDeleteBuffers(1, &p_vbo_normal);
        glDeleteBuffers(1, &p_vbo_texture);
        glDeleteBuffers(1, &p_vbo_index);
    }
}

void object_type::init( Transform *transform) {
    transform->setScale( p_size);
}

char *object_type::load_type( config *config, std::string l_path, std::string l_name) {
    XMLDocument l_file;
    bool l_idle = false;

    l_path = l_path + l_name + "/";

    std::string l_pathfile = l_path + (char*)DEFINITION_FILE;

    // if file don't exist - don't load
    if( file_exist( l_pathfile) == false)
        return "file don't found\n";

    // load the file
    XMLError l_result = l_file.LoadFile( l_pathfile.c_str());
    if ( l_result != XML_SUCCESS) {
        return "xml file was corrupt\n";
    }

    // get the object child element
    XMLElement* l_object = l_file.FirstChildElement( "object" );
    if( l_object == nullptr)
        return "no object child found\n";
    if( l_object->Attribute( "name"))
        p_name = l_object->Attribute( "name");
    else
        return "object name didn't found\n";

    // load file
    XMLElement* l_xml_file = l_object->FirstChildElement( "file" );
    if( !l_xml_file)
        return "object model file don't found\n";

    // get file name
    p_file = l_xml_file->GetText();
    if( !load_file( l_path + p_file))
        return "object model cant load\n";

    // size object
    double l_size = std::strtod( l_xml_file->Attribute( "size"), 0);
    p_size = glm::vec3( (float)l_size, (float)l_size, (float)l_size);

    // texture
    XMLElement* l_xml_texture = l_object->FirstChildElement( "texture" );
    if( l_xml_texture)
        p_texture_file = l_xml_texture->GetText();
    if( p_texture_file.size() > 0)
        p_texture = new texture( l_path +  p_texture_file);

    // size of hit box
    XMLElement* l_xml_hitbox = l_object->FirstChildElement( "hitbox" );
    if( l_xml_hitbox) {
        p_hitbox_offset.x = atof( l_xml_hitbox->Attribute( "x"));
        p_hitbox_offset.y = atof( l_xml_hitbox->Attribute( "y"));
        p_hitbox_offset.z = atof( l_xml_hitbox->Attribute( "z"));

        p_hitbox_size.x = atof( l_xml_hitbox->Attribute( "width"));
        p_hitbox_size.y = atof( l_xml_hitbox->Attribute( "height"));
        p_hitbox_size.z = atof( l_xml_hitbox->Attribute( "depth"));

        p_debug_draw.drawCube( p_hitbox_offset, p_hitbox_size, glm::vec3( 1, 0, 0));
    }
    return "";
}

bool object_type::load_file( std::string file) {
    std::string l_error;
    tinyobj::attrib_t l_attrib;
    std::vector<tinyobj::shape_t> l_shapes;
    std::vector<tinyobj::material_t> l_materials;

    p_indices.clear();

    // check if file exist
    std::ifstream l_file( file.c_str());
    if( l_file.fail())
        return false;
    else
        l_file.close();

    // load file
    bool l_return = tinyobj::LoadObj(&l_attrib, &l_shapes, &l_materials, &l_error, file.c_str());
    if( l_return == false) {
        printf( "%s\n", l_error.c_str());
        return false;
    }

    // Loop over shapes
    for (size_t s = 0; s < l_shapes.size(); s++) {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < l_shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = l_shapes[s].mesh.num_face_vertices[f];

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
          // access to vertex
          tinyobj::index_t idx = l_shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t l_vertex_x = l_attrib.vertices[3*idx.vertex_index+0];
          tinyobj::real_t l_vertex_y = l_attrib.vertices[3*idx.vertex_index+1];
          tinyobj::real_t l_vertex_z = l_attrib.vertices[3*idx.vertex_index+2];

          if( !l_attrib.texcoords.empty() ) {
            tinyobj::real_t tx = l_attrib.texcoords[2*idx.texcoord_index+0];
            tinyobj::real_t ty = 1.0f - l_attrib.texcoords[2*idx.texcoord_index+1];
            p_texcoords.push_back( glm::vec2( tx, ty));
          } else {
            p_texcoords.push_back( glm::vec2( 0, 0));
          }

          p_indices.push_back( p_indices.size() );
          p_vertices.push_back( glm::vec3( l_vertex_x, l_vertex_y, l_vertex_z));

          // Optional: vertex colors
          // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
          // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
          // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
        }
        index_offset += fv;
      }
    }

    // normal
    p_normal.resize( p_vertices.size());
    for( int i = 0; i < (int)p_indices.size(); i+=3) {
        glm::vec3 a(p_vertices[ p_indices[i+0]].x, p_vertices[p_indices[i+0]].y, p_vertices[p_indices[i+0]].z);
        glm::vec3 b(p_vertices[ p_indices[i+1]].x, p_vertices[p_indices[i+1]].y, p_vertices[p_indices[i+1]].z);
        glm::vec3 c(p_vertices[ p_indices[i+2]].x, p_vertices[p_indices[i+2]].y, p_vertices[p_indices[i+2]].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        p_normal[p_indices[i+0]] = normal;
        p_normal[p_indices[i+1]] = normal;
        p_normal[p_indices[i+2]] = normal;
    }

    p_model_changed = true;

    return true;
}

void object_type::updateVbo()
{
    if( p_model_changed == false)
        return;

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
    // create if not created
    if( p_vao == NULL) {
        glGenVertexArrays(1, &p_vao );
        glGenBuffers(1, &p_vbo_vertices);
        glGenBuffers(1, &p_vbo_normal);
        glGenBuffers(1, &p_vbo_texture);
        glGenBuffers(1, &p_vbo_index);
    }

    // update vao
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

void object_type::draw( glm::mat4 model, Shader* shader)
{
    if( p_vao == NULL)
        updateVao();

    updateVbo();

    if( p_texture)
        p_texture->Bind();

    shader->update( MAT_MODEL, model);

    // draw
    glBindVertexArray( p_vao);

    glDrawElements( GL_TRIANGLES, p_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void object_type::draw_debug( glm::mat4 model, Shader* shader) {
    shader->update( MAT_MODEL, model);

    p_debug_draw.draw( shader);
}

/*void object_type::setPhysic( b3Body *body)
{
    b3ShapeDef l_shapeDef;
    l_shapeDef.density = 2.0f;
    l_shapeDef.friction = 0.8f;


    b3CapsuleShape cs;
    cs.m_centers[0].Set(0.0f, 0.15f, 0.0f);
    cs.m_centers[1].Set(0.0f, -0.15f, 0.0f);
    cs.m_radius = 0.5f;

    l_shapeDef.shape = &p_hullDef; //p_hullDef

    body->CreateShape(l_shapeDef);

    //bdef.orientation.Set(b3Vec3(0.0f, 1.0f, 0.0f), 0.5f * B3_PI);

}*/

object::object()
{
    p_type = NULL;
//    p_body = NULL;
    p_model_change = false;
}

object::~object() {

}

void object::init()
{
    p_scale = p_type->getScale();
}

void object::process()
{
    // to do script
}

void object::process_phyisc() {
    glm::vec3 l_old_position;

    l_old_position = p_position;
    // add velocity
    p_position += p_velocity;

    //if( abs( p_position.y)-abs(l_old_position.y) > 0.01f );
    setUpdate();
}

void object::draw( Shader* shader) {
    if( p_type) {

        update_model();

        p_type->draw( p_model, shader);
    }
}

void object::draw_debug( Shader* shader) {
    if( p_type) {
        p_type->draw_debug( p_model, shader);
    }
}

void object::update_model() {
    if( !p_model_change)
        return;

    p_model_change = false;

    glm::mat4 l_posMat = glm::translate( p_position);
    glm::mat4 l_scaleMat = glm::scale( p_scale);
    glm::mat4 l_rotX = glm::rotate( p_rotation.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 l_rotY = glm::rotate( p_rotation.y, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 l_rotZ = glm::rotate( p_rotation.z, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 l_rotMat = l_rotX * l_rotY * l_rotZ;
    p_model = l_posMat * l_rotMat * l_scaleMat;
}

void object::setType( object_type *type)
{
    p_type = type;
    init();
}

void object::setPosition( glm::vec3 position)
{
    p_position = position;
    p_model_change = true;
}

void object::setRotation( glm::vec3 rotation)
{
    p_rotation = rotation;
    p_model_change = true;
}

/// object_handle
object_handle::object_handle()
{
    p_types.clear();
}

object_handle::~object_handle()
{
    /*for( auto l_type:p_types)
        delete l_type;*/

    p_types.clear();
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
        if( strlen( l_type->load_type( config, folder, l_entry->d_name)) > 0 ) {
            delete l_type;
            l_type = NULL;
            load_folder( l_file, config);
        }
        if( l_type) {
            printf( "object_handle::load_folder \"%s\" loaded\n", l_type->getName().c_str());
            p_types.push_back( l_type);
        }
    }
    closedir(l_dir);

    return true;
}

object_type *object_handle::get( std::string name) {
    for( int i = 0; i < (int)p_types.size(); i++)
        if( p_types[i]->getName() == name)
            return p_types[i];
    return NULL;
}
