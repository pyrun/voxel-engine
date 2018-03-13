#include "debug_draw.h"

void debug_draw::init( graphic *graphic) {
    p_shader = graphic->getDebugShader();
    p_vbo_lines = NULL;
    p_vbo_color = NULL;
    p_vao = NULL;

    p_vector_length = 1000;
    p_vector_size = 0;

    p_lines.reserve( p_vector_length);
    p_color.reserve( p_vector_length);

    p_change = true;
}

void debug_draw::drawLine( const btVector3& from, const btVector3& to, const btVector3& color)
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

void debug_draw::draw( glm::mat4 viewmatrix) {
    if( p_vector_size == 0)
        return;
    // create vbo and vao if not set
    if( !p_vbo_lines) {
        glGenBuffers( 1, &p_vbo_color);
        glGenBuffers( 1, &p_vbo_lines);
    }
    if( !p_vao) {
        glGenVertexArrays(1, &p_vao);

        glBindVertexArray(p_vao);

        glEnableVertexAttribArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(1);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindVertexArray(0);

        p_mvp = glGetUniformLocation( p_shader->GetProgram(), "g_mvp");
    }

    glUniformMatrix4fv( p_mvp, 1, GL_FALSE, glm::value_ptr( viewmatrix));
    glBindVertexArray(p_vao);

    if( p_change ) {
        p_change = false;
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( btVector3 ), &p_lines[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glBufferData( GL_ARRAY_BUFFER, p_vector_size * sizeof( btVector3 ), &p_color[0], GL_STATIC_DRAW);
    }

    glDrawArrays(GL_LINES, 0, p_vector_size);
    glBindVertexArray(0);

    p_vector_size = 0;
}
