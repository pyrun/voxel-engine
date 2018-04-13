#ifndef graphic_H
#define graphic_H

#define GLM_FORCE_ALIGNED


#include "display.h"
#include "shader.h"
#include "texture.h"
#include "light.h"

#define graphic_fov 1.2f
#define graphic_znear 0.01f
#define graphic_zfar 2000.0f

class graphic {
public:
    graphic( config *config);
    virtual ~graphic();

    void resizeWindow( int screen_width, int screen_height);

    void initShadowsMapping();
    void initDeferredShading();

    void resizeDeferredShading();

    void renderQuad();
    void renderDeferredShadingStart();
    void renderDeferredShadingEnd();

    void renderShadowmapStart();
    void renderShadowmapEnd();

    SDL_Surface* loadSurface(std::string File);
    void draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip);
    void saveImageBMP( std::string File);

    int createLight( glm::vec3 pos, glm::vec3 color);

    int getWidth() { return p_display->getWidth(); }
    int getHeight() { return p_display->getHeight(); }

    SDL_Window* getWindow() { if(p_display->getWindow() == NULL) printf( "graphic::GetWindow dont exist\n"); return p_display->getWindow(); }

    display * getDisplay() { if(p_display == NULL) printf( "graphic::GetDisplay dont exist\n"); return p_display; }

    Shader *getVoxelShader() { return p_voxel; }
    Shader *getObjectShader() { return p_object; }
    Shader *getGbuffer() { return p_gbuffer; }

    inline Camera *getCamera() { if(p_camera == NULL) printf( "graphic::getCamera dont exist\n"); return p_camera; }
protected:
private:
    Camera *p_camera;
    display* p_display;

    Shader *p_gbuffer;
    Shader *p_deferred_shading;

    unsigned int p_vao_quad;
    unsigned int p_vbo_quad;

    Shader* p_voxel;
    Shader *p_object;

    unsigned int p_fbo_buffer;
    unsigned int p_depth;
    unsigned int p_texture_position, p_texture_normal, p_texture_colorSpec;

    int p_index_light;
    std::vector<light> p_lights;

    const unsigned int p_shadow_width = 1024, p_shadow_height = 1024;
    unsigned int p_fbo_shadow_depth;
    unsigned int p_texture_shadow_depth;
};

#endif // graphic_H
