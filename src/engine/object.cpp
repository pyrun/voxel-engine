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

void object_type::init( Transform *transform) {
    transform->setScale( p_size);
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

    //
    XMLElement* l_object = l_file.FirstChildElement( "object" );
    if( l_object == nullptr)
        return false;

    if( l_object->Attribute( "name"))
        p_name = l_object->Attribute( "name");
    else
        return false;

    // load file
    XMLElement* l_xml_file = l_object->FirstChildElement( "file" );
    if( !l_xml_file)
        return false;
    p_file = l_xml_file->GetText();
    printf( "test %s\n", ( l_path + p_file).c_str() );
    if( !load_file( l_path + p_file))
        return false;

    printf( "test\n");

    double l_size = std::strtod( l_xml_file->Attribute( "size"), 0);
    p_size = glm::vec3( (float)l_size, (float)l_size, (float)l_size);
    printf( "%.2f\n", l_size);

    // texture
    XMLElement* l_xml_texture = l_object->FirstChildElement( "texture" );
    if( l_xml_texture)
        p_texture_file = l_xml_texture->GetText();
    if( p_texture_file.size() > 0)
        p_texture = new texture( l_path +  p_texture_file);

    printf( "l_object_name %s\n", p_name.c_str());
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
          tinyobj::real_t vx = l_attrib.vertices[3*idx.vertex_index+0];
          tinyobj::real_t vy = l_attrib.vertices[3*idx.vertex_index+1];
          tinyobj::real_t vz = l_attrib.vertices[3*idx.vertex_index+2];

          tinyobj::real_t nx = 0; //l_attrib.normals[3*idx.normal_index+0];
          tinyobj::real_t ny = 0; //l_attrib.normals[3*idx.normal_index+1];
          tinyobj::real_t nz = 0; //l_attrib.normals[3*idx.normal_index+2];

          tinyobj::real_t tx = l_attrib.texcoords[2*idx.texcoord_index+0];
          tinyobj::real_t ty = 1.0f - l_attrib.texcoords[2*idx.texcoord_index+1];

          p_indices.push_back( p_indices.size() );
          p_vertices.push_back( glm::vec3( vx, vy, vz));
          p_normal.push_back( glm::vec3( nx, ny, nz));
          p_texcoords.push_back( glm::vec2( tx, ty));
          // Optional: vertex colors
          // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
          // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
          // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
        }
        index_offset += fv;

        // per-face material
        //l_shapes[s].mesh.material_ids[f];
      }
    }

    /*ObjectCreator Obj;

    Obj.addCube( glm::vec3( 0, 2, 0), glm::vec3( 1, 2 ,1), glm::vec4( 0, 0, 1, 1));
            Obj.addCube( glm::vec3( 0, 0, 0), glm::vec3( 1, 1.5,1), glm::vec4( 0, 0, 1, 1));


    p_vertices = Obj.getVertices();
    p_indices = Obj.getIndices();
    p_texcoords = Obj.getTexcoords();
    p_normal.resize( p_vertices.size());
    for( int v = 0; v+3 <= p_vertices.size(); v+=3) {
        glm::vec3 a(p_vertices[v].x, p_vertices[v].y, p_vertices[v].z);
        glm::vec3 b(p_vertices[v+1].x, p_vertices[v+1].y, p_vertices[v+1].z);
        glm::vec3 c(p_vertices[v+2].x, p_vertices[v+2].y, p_vertices[v+2].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 l_normal = glm::normalize( glm::cross( edge1, edge2));

        p_normal[v] = l_normal;
        p_normal[v+1] = l_normal;
        p_normal[v+2] = l_normal;
    }*/

    return true;
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

void object_type::draw( glm::mat4 model, Shader* shader, glm::mat4 viewprojection)
{
    updateVbo();
    updateVao();

    shader->Bind();

    if( p_texture)
        p_texture->Bind();

    shader->update( model, viewprojection);
    // draw
    glBindVertexArray( p_vao);

    glDrawElements( GL_TRIANGLES, p_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

btRigidBody *object_type::makeBulletMesh() {
    btRigidBody * body = nullptr;

    // Handy lambda for converting from irr::vector to btVector
    auto toBtVector = [ &]( const glm::vec3 & vec ) -> btVector3
    {
        btVector3 bt( vec.x, vec.y, vec.z );

        return bt;
    };

    // Make bullet rigid body
    if ( ! p_vertices.empty() && ! p_indices.empty() )
    {
        // Working numbers
        const size_t numIndices     = p_indices.size();
        const size_t numTriangles   = numIndices / 3;

        // Create triangles
        btTriangleMesh * btmesh = new btTriangleMesh();

        // Build btTriangleMesh
        for ( size_t i=0; i<numIndices; i+=3 )
        {
            const btVector3 &A = toBtVector( p_vertices[ p_indices[ i+0 ] ] );
            const btVector3 &B = toBtVector( p_vertices[ p_indices[ i+1 ] ] );
            const btVector3 &C = toBtVector( p_vertices[ p_indices[ i+2 ] ] );

            bool removeDuplicateVertices = true;
            btmesh->addTriangle( A, B, C, removeDuplicateVertices );
        }

        // Give it a default MotionState
        btDefaultMotionState* fallMotionState =
                new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(10, 10, 0)));

        btConvexShape  *tmpshape = new btConvexTriangleMeshShape( btmesh );
        btShapeHull *hull = new btShapeHull(tmpshape);
        btScalar margin = tmpshape->getMargin();
        hull->buildHull(margin);
        btConvexHullShape* simplifiedConvexShape = new btConvexHullShape( (btScalar*)hull->getVertexPointer(), hull->numVertices());

        //btConvexTriangleMeshShape *tmpshape2 = new btConvexTriangleMeshShape( btmesh);
        btGImpactMeshShape *tmpshape2 = new btGImpactMeshShape( btmesh);
        tmpshape2->updateBound();

        btScalar mass = 100;
        btVector3 fallInertia(0, 0, 0);
        //btShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, tmpshape2, fallInertia);
        body = new btRigidBody(fallRigidBodyCI);

    }
    return body;
}

object::object()
{
    p_type = NULL;
}

object::~object() {

}

void object::init()
{
    if( p_type)
        p_transform.setScale( p_type->getScale());
}

void object::draw( Shader* shader, glm::mat4 viewprojection) {
    if( p_type)
        ;//p_type->draw( getTransform(), shader, viewprojection);
}

object_handle::object_handle()
{

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
        if( l_type->load_type( config, folder, l_entry->d_name) == false) {
            delete l_type;
            load_folder( l_file, config);
        }
        p_types.push_back( l_type);
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
