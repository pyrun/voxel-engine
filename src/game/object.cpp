#include "object.h"

Object::Object() {
    // Buffer erzeugen
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_vboTexture);
    glGenBuffers(1, &m_vboIndex);

}

Object::~Object() {
    // VBO löschen
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_vboTexture);
    glDeleteBuffers(1, &m_vboIndex);

}
