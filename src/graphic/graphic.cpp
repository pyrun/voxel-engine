#include "graphic.h"


#include <SDL2/SDL_image.h>

graphic::graphic( int Width, int Height) {
    //
    p_display = new display( Width, Height, "Selur");

    // Shader laden
    p_voxel = new Shader( "shader/voxels");

    // Shadow
    p_shadow = new Shader( "shader/shadow");

    // Vertex shader
    p_vertex = new Shader( "shader/vertex");

    // object shader
    p_object = new Shader( "shader/object");

    // camera
    p_camera = new Camera(glm::vec3( -0.5f, 0.0f, -0.5f), graphic_fov, (float)Width/(float)Height, graphic_znear, graphic_zfar);
}

graphic::~graphic() {
    delete p_camera;
    delete p_shadow;
    delete p_voxel;
    delete p_vertex;
    delete p_display;
}

void graphic::resizeWindow( int screen_width, int screen_height) {
    glViewport(0, 0, screen_width, screen_height);
    printf( "%d %d resize\n", screen_width, screen_height);
    p_display->setSize( screen_width, screen_height);
    // resize
    p_camera->resize( (float)screen_width/(float)screen_height);
}

SDL_Surface* graphic::loadSurface(std::string File) {
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

void graphic::draw( SDL_Surface* Image, double X, double Y, int W, int H, int SpriteX, int SpriteY, bool Flip) {
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
    if( p_display->getSurface() == NULL )
        printf( "Screen dont load...\n");
    SDL_BlitSurface( Image, &source, p_display->getSurface(), &destination);
}

void graphic::saveImageBMP( std::string File) {
    // Save Image
    File = File + ".png";
    // Image save
    if( IMG_SavePNG( p_display->getSurface(), File.c_str())) {
        printf("Unable to save png -- %s\n", SDL_GetError());
    }
}
