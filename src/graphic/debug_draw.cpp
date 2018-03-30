#include "debug_draw.h"

debug_draw::debug_draw() {
    p_vbo_lines = 0;
    p_vbo_color = 0;
    p_vao = 0;

    p_vector_length = 1000;
    p_vector_size = 0;

    p_lines.reserve( p_vector_length);
    p_color.reserve( p_vector_length);

    p_change = true;
}

debug_draw::~debug_draw() {
    if( p_vbo_lines) {
        glDeleteVertexArrays(1, &p_vao );
        glDeleteBuffers(1, &p_vbo_color);
        glDeleteBuffers(1, &p_vbo_lines);
    }
}

void debug_draw::drawLine( glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
    if( p_vector_length <= p_vector_size+100) {
        p_vector_length+=100;
        p_lines.reserve( p_vector_length);
        p_color.reserve( p_vector_length);
    }

    p_lines[ p_vector_size] = from;
    p_lines[ p_vector_size+1] = to;
    p_color[ p_vector_size] = color;
    p_color[ p_vector_size+1] = color;
    p_vector_size += 2;

    p_change = true;
}

void debug_draw::drawCube( glm::vec3 from, glm::vec3 size, glm::vec3 color)
{
    // cube
    drawLine( from + glm::vec3( 0, 0, 0),
              from + glm::vec3( size.x, 0, 0), color );
    drawLine( from + glm::vec3( 0, 0, 0),
              from + glm::vec3( 0, 0, size.z), color );
    drawLine( from + glm::vec3( size.x, 0, size.z),
              from + glm::vec3( size.x, 0, 0), color );
    drawLine( from + glm::vec3( 0, 0, size.z),
              from + glm::vec3( size.x, 0, size.z), color );
}

void debug_draw::draw( glm::mat4 viewmatrix, Shader *shader) {
    if( p_vector_size == 0)
        return;
    // create vbo and vao if not set
    if( !p_vbo_lines) {
        glGenBuffers( 1, &p_vbo_color);
        glGenBuffers( 1, &p_vbo_lines);
    }
    if( !p_vao) {
        glGenVertexArrays(1, &p_vao);

        glBindVertexArray( p_vao);

        glEnableVertexAttribArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(1);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindVertexArray(0);

        p_mvp = glGetUniformLocation( shader->GetProgram(), "g_mvp");
    }

    glUniformMatrix4fv( p_mvp, 1, GL_FALSE, glm::value_ptr( viewmatrix));
    glBindVertexArray( p_vao);

    if( p_change ) {
        p_change = false;
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( glm::vec3 ), &p_lines[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( glm::vec3 ), &p_color[0], GL_STATIC_DRAW);
    }

    glDrawArrays(GL_LINES, 0, p_vector_size);
    glBindVertexArray(0);

    p_vector_size = 0;
}
