#include "objectcreator.h"

ObjectCreator::ObjectCreator() {
    //ctor
}

ObjectCreator::~ObjectCreator() {
    //dtor
}

/*void ObjectCreator::addTexture( glm::vec2 l_start, glm::vec2 l_end, int flip) {
}*/


void ObjectCreator::addIndices( int l_vertices_end, bool l_flip) {
    // indices
    if( l_flip == false ) {
        m_indices.push_back( l_vertices_end + 0 );
        m_indices.push_back( l_vertices_end + 1 );
        m_indices.push_back( l_vertices_end + 2 );
        m_indices.push_back( l_vertices_end + 0 );
        m_indices.push_back( l_vertices_end + 2 );
        m_indices.push_back( l_vertices_end + 3 );
    } else {
        m_indices.push_back( l_vertices_end + 2 );
        m_indices.push_back( l_vertices_end + 1 );
        m_indices.push_back( l_vertices_end + 0 );
        m_indices.push_back( l_vertices_end + 3 );
        m_indices.push_back( l_vertices_end + 2 );
        m_indices.push_back( l_vertices_end + 0 );
    }
}

void ObjectCreator::addCube( glm::vec3 l_pos, glm::vec3 l_size, glm::vec4 l_texture) {
    addXPlate( l_pos, l_size, l_texture);
    addXPlate( l_pos, l_size, l_texture, true);
    addZPlate( l_pos, l_size, l_texture, true);
    addZPlate( l_pos, l_size, l_texture);
    addYPlate( l_pos, l_size, l_texture, true);
    addYPlate( l_pos, l_size, l_texture);
}

void ObjectCreator::addXPlate( glm::vec3 l_pos, glm::vec3 l_size, glm::vec4 l_texture, bool flip) {
    int l_vertices_end = m_vertices.size();
    addIndices( l_vertices_end, flip);
    if( flip == false)
        l_size.z = 0;
    // plate x
    m_vertices.push_back( l_pos + glm::vec3( 0.0f, 0.0f, l_size.z) ); // 0
    m_vertices.push_back( l_pos + glm::vec3( 0.0f, l_size.y, l_size.z) ); // 1
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, l_size.y, l_size.z) ); // 2
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, 0.0f, l_size.z) ); // 3
    // texture
    if( flip) {
        float l_texture_temp = l_texture.z;
        l_texture.z = l_texture.x;
        l_texture.x = l_texture_temp;
    }
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.w) );
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.y) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.y) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.w) );

}

void ObjectCreator::addZPlate( glm::vec3 l_pos, glm::vec3 l_size, glm::vec4 l_texture, bool flip) {
    int l_vertices_end = m_vertices.size();
    addIndices( l_vertices_end, flip);
    if( flip == false) {
        l_size.x = 0;
    }
    // plate y
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, 0.0f, l_size.z) ); // 0
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, l_size.y, l_size.z) ); // 1
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, l_size.y, 0.0) ); // 2
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, 0.0f, 0.0) ); // 3

    // texture
    if( flip) {
        float l_texture_temp = l_texture.z;
        l_texture.z = l_texture.x;
        l_texture.x = l_texture_temp;
    }
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.w) );
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.y) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.y) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.w) );
}

void ObjectCreator::addYPlate( glm::vec3 l_pos, glm::vec3 l_size, glm::vec4 l_texture, bool flip) {
    int l_vertices_end = m_vertices.size();
    addIndices( l_vertices_end, !flip);
    if( flip == false) {
        l_size.y = 0;
    }
    // plate y
    m_vertices.push_back( l_pos + glm::vec3( 0.0f, l_size.y, l_size.z) ); // 0
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, l_size.y, l_size.z) ); // 1
    m_vertices.push_back( l_pos + glm::vec3( l_size.x, l_size.y, 0.0) ); // 2
    m_vertices.push_back( l_pos + glm::vec3( 0.0f, l_size.y, 0.0) ); // 3
    // texture
    if( flip) {
        /*float l_texture_temp = l_texture.z;
        l_texture.z = l_texture.x;
        l_texture.x = l_texture_temp;*/

        float l_texture_temp = l_texture.w;
        l_texture.w = l_texture.y;
        l_texture.y = l_texture_temp;
    }
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.y) );
    m_texcoords.push_back( glm::vec2( l_texture.z, l_texture.w) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.w) );
    m_texcoords.push_back( glm::vec2( l_texture.x, l_texture.y) );
}
