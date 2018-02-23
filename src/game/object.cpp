#include "object.h"

Object::Object() {
    // Buffer erzeugen
    glGenVertexArrays(1, &p_vao );
    glGenBuffers(1, &p_vbo);
    glGenBuffers(1, &p_vboNormal);
    glGenBuffers(1, &p_vboTexture);
    glGenBuffers(1, &p_vboIndex);
}

Object::~Object() {
    // VBO löschen
    glDeleteVertexArrays(1, &p_vao );
    glDeleteBuffers(1, &p_vbo);
    glDeleteBuffers(1, &p_vboNormal);
    glDeleteBuffers(1, &p_vboTexture);
    glDeleteBuffers(1, &p_vboIndex);

}
