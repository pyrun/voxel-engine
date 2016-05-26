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

    int GetWidth() { return m_display->GetWidth(); }
    int GetHeight() { return m_display->GetHeight(); }

    SDL_Window* GetWindow() { if(m_display->GetWindow() == NULL) printf( "Graphic::GetWindow dont exist\n"); return m_display->GetWindow(); }

    Display * GetDisplay() { if(m_display == NULL) printf( "Graphic::GetDisplay dont exist\n"); return m_display; }

    inline Shader *GetVoxelShader() { if(m_voxel == NULL) printf( "Graphic::GetVoxelShader dont exist\n"); return m_voxel; }
    inline Shader *GetVertexShader() { if(m_vertex== NULL) printf( "Graphic::GetVertexShader dont exist\n"); return m_vertex; }
    inline Shader *GetShadowShader() { if(m_shadow== NULL) printf( "Graphic::GetShadowShader dont exist\n"); return m_shadow; }
    inline Shader *GetObjectShader() { if(m_object== NULL) printf( "Graphic::GetObjectShader dont exist\n"); return m_object; }
    inline Camera *GetCamera() { if(m_camera == NULL) printf( "Graphic::GetCamera dont exist\n"); return m_camera; }
protected:
private:
    Camera *m_camera;
    Display* m_display;
    Shader* m_voxel;
    Shader* m_vertex;
    Shader *m_shadow;
    Shader *m_object;
};

#endif // GRAPHIC_H
