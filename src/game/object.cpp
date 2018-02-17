#include "object.h"

Object::Object() {
    // Buffer erzeugen
    glGenBuffers(1, &p_vbo);
    glGenBuffers(1, &p_vboTexture);
    glGenBuffers(1, &p_vboIndex);

}

Object::~Object() {
    // VBO löschen
    glDeleteBuffers(1, &p_vbo);
    glDeleteBuffers(1, &p_vboTexture);
    glDeleteBuffers(1, &p_vboIndex);

}
