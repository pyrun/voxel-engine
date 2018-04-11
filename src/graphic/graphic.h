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

    void initDeferredShading();
    void deferredShading();

    SDL_Surface* loadSurface(std::string File);
    void draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip);
    void saveImageBMP( std::string File);

    int createLight( glm::vec3 pos, glm::vec3 color);

    int getWidth() { return p_display->getWidth(); }
    int getHeight() { return p_display->getHeight(); }

    SDL_Window* getWindow() { if(p_display->getWindow() == NULL) printf( "graphic::GetWindow dont exist\n"); return p_display->getWindow(); }

    display * getDisplay() { if(p_display == NULL) printf( "graphic::GetDisplay dont exist\n"); return p_display; }

    inline Shader *getVoxelShader() { if(p_voxel == NULL) printf( "graphic::GetVoxelShader dont exist\n"); return p_voxel; }
    inline Shader *getObjectShader() { if(p_object== NULL) printf( "graphic::GetObjectShader dont exist\n"); return p_object; }
    inline Shader *getDebugShader() { if(p_debug== NULL) printf( "graphic::getDebugShader dont exist\n"); return p_debug; }

    unsigned int getBufferFbo() { return p_buffer; }

    Shader *getGbuffer() { return p_gbuffer; }

    inline Camera *getCamera() { if(p_camera == NULL) printf( "graphic::getCamera dont exist\n"); return p_camera; }
protected:
private:
    Camera *p_camera;
    display* p_display;

    Shader *p_gbuffer;
    Shader *p_deferred_shading;

    Shader* p_voxel;
    Shader *p_object;
    Shader *p_debug;

    unsigned int p_buffer;
    unsigned int p_depth;
    unsigned int p_position, p_normal, p_colorSpec;

    int p_index_light;
    std::vector<light> p_lights;
};

#endif // graphic_H
