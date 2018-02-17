#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "display.h"
#include "shader.h"
#include "texture.h"

#define graphic_fov 2.0f
#define graphic_znear 0.01f
#define graphic_zfar 1000.0f

class Graphic {
public:
    Graphic( int Width = 640, int Height = 400);
    virtual ~Graphic();

    void ResizeWindow( int screen_width, int screen_height);

    SDL_Surface* LoadSurface(std::string File);
    void Draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip);
    void SaveImageBMP( std::string File);

    int GetWidth() { return p_display->GetWidth(); }
    int GetHeight() { return p_display->GetHeight(); }

    SDL_Window* GetWindow() { if(p_display->GetWindow() == NULL) printf( "Graphic::GetWindow dont exist\n"); return p_display->GetWindow(); }

    Display * GetDisplay() { if(p_display == NULL) printf( "Graphic::GetDisplay dont exist\n"); return p_display; }

    inline Shader *GetVoxelShader() { if(p_voxel == NULL) printf( "Graphic::GetVoxelShader dont exist\n"); return p_voxel; }
    inline Shader *GetVertexShader() { if(p_vertex== NULL) printf( "Graphic::GetVertexShader dont exist\n"); return p_vertex; }
    inline Shader *GetShadowShader() { if(p_shadow== NULL) printf( "Graphic::GetShadowShader dont exist\n"); return p_shadow; }
    inline Shader *GetObjectShader() { if(p_object== NULL) printf( "Graphic::GetObjectShader dont exist\n"); return p_object; }
    inline Camera *GetCamera() { if(p_camera == NULL) printf( "Graphic::GetCamera dont exist\n"); return p_camera; }
protected:
private:
    Camera *p_camera;
    Display* p_display;
    Shader* p_voxel;
    Shader* p_vertex;
    Shader *p_shadow;
    Shader *p_object;
};

#endif // GRAPHIC_H
