#include "debug_draw.h"

void debug_draw::init( graphic *graphic) {
    p_shader = graphic->getDebugShader();
    p_vbo_lines = NULL;
    p_vbo_color = NULL;
    p_vao = NULL;

    p_lines.reserve( 100*100);
    p_color.reserve( 100*100);
}

void debug_draw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    glm::vec3 l_line_1 = glm::vec3( from.getX(), (float)from.getY(), (float)from.getZ());
    glm::vec3 l_line_2 = glm::vec3( to.getX(), to.getY(), to.getZ());
    glm::vec3 l_color_1 = glm::vec3( color.getX(), color.getY(), color.getZ());
    glm::vec3 l_color_2 = l_color_1;


    p_lines.push_back( l_line_1);
    p_lines.push_back( l_line_2);
    p_color.push_back( l_color_1);
    p_color.push_back( l_color_2);
}

void debug_draw::draw( glm::mat4 viewmatrix) {
    if( p_lines.size() == 0)
        return;
    // create vbo and vao if not set
    if( !p_vbo_lines) {
        glGenBuffers( 1, &p_vbo_color);
        glGenBuffers( 1, &p_vbo_lines);
    }
    if( !p_vao) {
        glGenVertexArrays(1, &p_vao);

        glBindVertexArray(p_vao);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glBufferData( GL_ARRAY_BUFFER, p_lines.size() * sizeof( glm::vec3 ), &p_lines[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glBufferData( GL_ARRAY_BUFFER, p_color.size() * sizeof( glm::vec3 ), &p_color[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glEnableVertexAttribArray(1);
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

        glBindVertexArray(0);
    } else {
        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_lines);
        glBufferData( GL_ARRAY_BUFFER, p_lines.size() * sizeof( glm::vec3 ), &p_lines[0], GL_STATIC_DRAW);

        glBindBuffer( GL_ARRAY_BUFFER, p_vbo_color);
        glBufferData( GL_ARRAY_BUFFER, p_color.size() * sizeof( glm::vec3 ), &p_color[0], GL_STATIC_DRAW);
    }
    glUniformMatrix4fv(glGetUniformLocation( p_shader->GetProgram(), "g_mvp"), 1, GL_FALSE, glm::value_ptr( viewmatrix));
    glBindVertexArray(p_vao);
    glDrawArrays(GL_LINES, 0, p_lines.size());
    glBindVertexArray(0);

    p_lines.clear();
    p_color.clear();
}
