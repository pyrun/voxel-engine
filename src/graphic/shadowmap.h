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

    bool IsStarted() { return m_started; }

    glm::vec2 GetSize() { return m_size; }
private:
    GLuint m_fbo;
    GLuint m_shadowMap;
    bool m_started;
    glm::vec2 m_size;
};

#endif // SHADOWMAP_H
