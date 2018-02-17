#include <stdio.h>

#include "shadowmap.h"


ShadowMap::ShadowMap() {
    p_fbo = 0;
    p_shadowMap = 0;
    p_started = false;
}

ShadowMap::~ShadowMap() {
    if (p_fbo != 0) {
        glDeleteFramebuffers(1, &p_fbo);
    }

    if (p_shadowMap != 0) {
        glDeleteTextures(1, &p_shadowMap);
    }
}

bool ShadowMap::Init(unsigned int WindowWidth, unsigned int WindowHeight) {
     // Create the FBO
    glGenFramebuffers(1, &p_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, p_fbo);
    // Create the depth buffer
    glGenTextures(1, &p_shadowMap);
    glBindTexture(GL_TEXTURE_2D, p_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, p_shadowMap, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    p_started = true;

    return true;
/*    GLenum FBOstatus;

	  // Try to use a texture depth component
	  glGenTextures(1, &p_shadowMap);
	  glBindTexture(GL_TEXTURE_2D, p_shadowMap);

	  // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	  // Remove artifact on the edges of the shadowmap
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	  // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
	  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	  glBindTexture(GL_TEXTURE_2D, 0);

	  // create a framebuffer object
	  glGenFramebuffersEXT(1, &p_fbo);
	  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, p_fbo);

	  // Instruct openGL that we won't bind a color texture with the currently bound FBO
	  glDrawBuffer(GL_NONE);
	  glReadBuffer(GL_NONE);

	  // attach the texture to FBO depth attachment point
	  //glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, p_shadowMap, 0);
	  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, p_shadowMap, 0);

	  // check FBO status
	  FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		  printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	  // switch back to window-system-provided framebuffer
	  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);*/
}

void ShadowMap::setTextureMatrix(void) {
    static double modelView[16];
    static double projection[16];

    // Moving from unit cube [-1,1] to [0,1]
    const GLdouble bias[16] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

    // Grab modelview and transformation matrices
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);


    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(GL_TEXTURE7);

    glLoadIdentity();
    glLoadMatrixd(bias);

    // concatating all matrices into one.
    glMultMatrixd (projection);
    glMultMatrixd (modelView);

    // Go back to normal matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void ShadowMap::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, p_fbo);
}

void ShadowMap::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, p_shadowMap);
}
