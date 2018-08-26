#include "entity_draw.h"

entity_draw::entity_draw() {
    p_vbo_vertex = 0;
    p_vbo_normal = 0;
    p_vbo_color = 0;
    p_vao = 0;

    // disable
    disable();

    p_change = true;
}

entity_draw::~entity_draw() {
    // clearing the array
    clear();

    // deleting vbo
    if( p_vbo_vertex) {
        glDeleteVertexArrays(1, &p_vao );
        glDeleteBuffers( 1, &p_vbo_indices);
        glDeleteBuffers(1, &p_vbo_vertex);
        glDeleteBuffers(1, &p_vbo_normal);
        glDeleteBuffers(1, &p_vbo_color);
    }
}

void entity_draw::cube( glm::vec3 positon, glm::vec3 size, glm::vec3 color) {
    // add vertex
    int l_vertex_begin = p_vertex.size();
    p_vertex.push_back( positon + glm::vec3( size.x, size.y, size.z)); //0
    p_vertex.push_back( positon + glm::vec3(   0.0f, size.y, size.z)); //1
    p_vertex.push_back( positon + glm::vec3( size.x, size.y,   0.0f)); //2
    p_vertex.push_back( positon + glm::vec3(   0.0f, size.y,   0.0f)); //3
    p_vertex.push_back( positon + glm::vec3( size.x,   0.0f, size.z)); //4
    p_vertex.push_back( positon + glm::vec3(   0.0f,   0.0f, size.z)); //5
    p_vertex.push_back( positon + glm::vec3(   0.0f,   0.0f,   0.0f)); //6
    p_vertex.push_back( positon + glm::vec3( size.x,   0.0f,   0.0f)); //7

    for( int i= 0; i < 8; i++)
        p_color.push_back( color);

    // add indices
    int l_indices_begin = p_indices.size();
    p_indices.push_back( 3 + l_vertex_begin);
    p_indices.push_back( 2 + l_vertex_begin);
    p_indices.push_back( 6 + l_vertex_begin);
    p_indices.push_back( 6 + l_vertex_begin);
    p_indices.push_back( 2 + l_vertex_begin);
    p_indices.push_back( 7 + l_vertex_begin);

    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 7 + l_vertex_begin);
    p_indices.push_back( 2 + l_vertex_begin);
    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 2 + l_vertex_begin);
    p_indices.push_back( 0 + l_vertex_begin);

    p_indices.push_back( 0 + l_vertex_begin);
    p_indices.push_back( 3 + l_vertex_begin);
    p_indices.push_back( 1 + l_vertex_begin);
    p_indices.push_back( 0 + l_vertex_begin);
    p_indices.push_back( 2 + l_vertex_begin);
    p_indices.push_back( 3 + l_vertex_begin);

    p_indices.push_back( 5 + l_vertex_begin);
    p_indices.push_back( 6 + l_vertex_begin);
    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 6 + l_vertex_begin);
    p_indices.push_back( 7 + l_vertex_begin);

    p_indices.push_back( 6 + l_vertex_begin);
    p_indices.push_back( 5 + l_vertex_begin);
    p_indices.push_back( 3 + l_vertex_begin);
    p_indices.push_back( 3 + l_vertex_begin);
    p_indices.push_back( 5 + l_vertex_begin);
    p_indices.push_back( 1 + l_vertex_begin);

    p_indices.push_back( 1 + l_vertex_begin);
    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 0 + l_vertex_begin);

    p_indices.push_back( 5 + l_vertex_begin);
    p_indices.push_back( 4 + l_vertex_begin);
    p_indices.push_back( 1 + l_vertex_begin);

    for( int i = l_indices_begin; i < p_vertex.size(); i+=3) {
        glm::vec3 a(p_vertex[ p_indices[i+0]].x, p_vertex[p_indices[i+0]].y, p_vertex[p_indices[i+0]].z);
        glm::vec3 b(p_vertex[ p_indices[i+1]].x, p_vertex[p_indices[i+1]].y, p_vertex[p_indices[i+1]].z);
        glm::vec3 c(p_vertex[ p_indices[i+2]].x, p_vertex[p_indices[i+2]].y, p_vertex[p_indices[i+2]].z);
        glm::vec3 edge1 = b-a;
        glm::vec3 edge2 = c-a;
        glm::vec3 normal = glm::normalize( glm::cross( edge1, edge2));

        p_normal.push_back( normal);
        p_normal.push_back( normal);
        p_normal.push_back( normal);
    }

    return;
}

void entity_draw::clear() {
    p_vertex.clear();
    p_color.clear();
    p_indices.clear();
    p_normal.clear();
}

void entity_draw::setChange() {
    p_change = true;
    enable();
}

void entity_draw::draw( Shader *shader) {
    if( p_indices.size() == 0 || p_enable == false)
        return;
    // create vbo and vao if not set
    if( !p_vbo_vertex) {
        glGenBuffers( 1, &p_vbo_indices);
        glGenBuffers( 1, &p_vbo_vertex);
        glGenBuffers( 1, &p_vbo_normal);
        glGenBuffers( 1, &p_vbo_color);
    }
    if( !p_vao) {
        glGenVertexArrays(1, &p_vao);

        glBindVertexArray( p_vao);

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vbo_indices);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof( unsigned int ), &p_indices[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertex);
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(1);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(2);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindVertexArray(0);
    }

    //glUniformMatrix4fv( p_mvp, 1, GL_FALSE, glm::value_ptr( viewmatrix));
    glBindVertexArray( p_vao);

    if( p_change ) {
        p_change = false;
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_vbo_indices);
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, p_indices.size() * sizeof( unsigned int ), &p_indices[0], GL_DYNAMIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_vertex);
        glBufferData( GL_ARRAY_BUFFER, p_vertex.size() * sizeof( glm::vec3 ), &p_vertex[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_normal);
        glBufferData( GL_ARRAY_BUFFER, p_normal.size() * sizeof( glm::vec3 ), &p_normal[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glBufferData( GL_ARRAY_BUFFER, p_color.size() * sizeof( glm::vec3 ), &p_color[0], GL_STATIC_DRAW);
    }

    glDrawElements( GL_TRIANGLES, p_indices.size(), GL_UNSIGNED_INT, 0);

    //glDrawArrays( GL_TRIANGLES, 0, p_vector_size);

    glBindVertexArray(0);
}
