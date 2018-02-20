#ifndef graphic_H
#define graphic_H

#define GLM_FORCE_ALIGNED

#include "display.h"
#include "shader.h"
#include "texture.h"

#define graphic_fov 2.0f
#define graphic_znear 0.01f
#define graphic_zfar 1000.0f

class graphic {
public:
    graphic( int Width = 640, int Height = 400);
    virtual ~graphic();

    void resizeWindow( int screen_width, int screen_height);

    SDL_Surface* loadSurface(std::string File);
    void draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip);
    void saveImageBMP( std::string File);

    int getWidth() { return p_display->getWidth(); }
    int getHeight() { return p_display->getHeight(); }

    SDL_Window* getWindow() { if(p_display->getWindow() == NULL) printf( "graphic::GetWindow dont exist\n"); return p_display->getWindow(); }

    display * getDisplay() { if(p_display == NULL) printf( "graphic::GetDisplay dont exist\n"); return p_display; }

    inline Shader *getVoxelShader() { if(p_voxel == NULL) printf( "graphic::GetVoxelShader dont exist\n"); return p_voxel; }
    inline Shader *getVertexShader() { if(p_vertex== NULL) printf( "graphic::getVertexShader dont exist\n"); return p_vertex; }
    inline Shader *getShadowShader() { if(p_shadow== NULL) printf( "graphic::GetShadowShader dont exist\n"); return p_shadow; }
    inline Shader *getObjectShader() { if(p_object== NULL) printf( "graphic::GetObjectShader dont exist\n"); return p_object; }
    inline Camera *getCamera() { if(p_camera == NULL) printf( "graphic::getCamera dont exist\n"); return p_camera; }
protected:
private:
    Camera *p_camera;
    display* p_display;
    Shader* p_voxel;
    Shader* p_vertex;
    Shader *p_shadow;
    Shader *p_object;
};

#endif // graphic_H
