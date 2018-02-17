#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <GL/glew.h>

#include "camera.h"

class ShadowMap {
public:
    ShadowMap();
    ~ShadowMap();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
    void setTextureMatrix(void);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

    bool IsStarted() { return p_started; }

    glm::vec2 GetSize() { return p_size; }
private:
    GLuint p_fbo;
    GLuint p_shadowMap;
    bool p_started;
    glm::vec2 p_size;
};

#endif // SHADOWMAP_H
