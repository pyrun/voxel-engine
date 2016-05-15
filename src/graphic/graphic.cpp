#include "graphic.h"


#include <SDL2/SDL_image.h>

Graphic::Graphic( int Width, int Height) {
    //
    m_display = new Display( Width, Height, "Selur");

    // Shader laden
    m_voxel = new Shader( "shader/voxels");

    // Shadow
    m_shadow = new Shader( "shader/shadow");

    // Vertex shader
    m_vertex = new Shader( "shader/vertex");

    // object shader
    m_object = new Shader( "shader/object");

    // camera
    m_camera = new Camera(glm::vec3( -0.5f, 0.0f, -0.5f), graphic_fov, (float)Width/(float)Height, graphic_znear, graphic_zfar);
}

Graphic::~Graphic() {
    delete m_camera;
    delete m_shadow;
    delete m_voxel;
    delete m_vertex;
    delete m_display;
}

void Graphic::ResizeWindow( int screen_width, int screen_height) {
    glViewport(0, 0, screen_width, screen_height);
    printf( "%d %d resize\n", screen_width, screen_height);
    m_display->SetSize( screen_width, screen_height);
    // resize
    m_camera->GetProjection() = glm::perspective( graphic_fov, (float)screen_width/(float)screen_height, graphic_znear, graphic_zfar);
}

SDL_Surface* Graphic::LoadSurface(std::string File) {
    // Laden der Datei
    SDL_Surface* loaded = IMG_Load(File.c_str());
	if( loaded == NULL ) {
        printf( "Unable to load image %s!", File.c_str() ); // Fehler
        printf( "%s", SDL_GetError() );
        return NULL;
    } else {
        // Color key ferstlegen
        //SDL_SetColorKey( loaded, true, SDL_MapRGB( loaded->format, 200, 191, 231));
    }
    return loaded;
}

void Graphic::Draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip) {
    SDL_Rect destination;
    destination.x = (int)X;
    destination.y = (int)Y;
    destination.w = W;
    destination.h = H;
    SDL_Rect source;
    source.x = SpriteX*W;
    source.y = SpriteY*H;
    source.w = W;
    source.h = H;
    if( Image == NULL )
        printf( "Image dont load...\n");
    if( m_display->GetSurface() == NULL )
        printf( "Screen dont load...\n");
    SDL_BlitSurface( Image, &source, m_display->GetSurface(), &destination);
}

void Graphic::SaveImageBMP( std::string File) {
    // Save Image
    File = File + ".png";
    /*SDL_RWops *file = SDL_RWFromFile( File.c_str(), "wb");
    SDL_SaveBMP_RW( m_display->GetSurface(), file, 0);
    file->close(file);*/
    if( IMG_SavePNG( m_display->GetSurface(), File.c_str())) {
        printf("Unable to save png -- %s\n", SDL_GetError());
    }
}
