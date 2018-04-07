#include "graphic.h"
#include <SDL2/SDL_image.h>

graphic::graphic( config *config) {
    //
    p_display = new display( config);

    // Shader laden
    p_voxel = new Shader( "shader/voxels");
    p_vertex = new Shader( "shader/vertex");
    p_object = new Shader( "shader/object");
    p_debug = new Shader( "shader/debug");

    // camera
    p_camera = new Camera(glm::vec3( -0.5f, 0.0f, -0.5f), graphic_fov, (float)p_display->getWidth()/(float)p_display->getHeight(), graphic_znear, graphic_zfar);

    initDeferredShading();

    p_index_light = 0;

    createLight( glm::vec3( 0, 20, 0), glm::vec3( 1, 0, 0));
}

graphic::~graphic() {
    delete p_debug;
    delete p_camera;
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

void graphic::initDeferredShading() {
    glm::vec2 l_scrn = p_display->getSize();


    glGenFramebuffers(1, &p_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, p_buffer);

    // 1 - position color buffer
    glGenTextures(1, &p_position);
    glBindTexture(GL_TEXTURE_2D, p_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, l_scrn.x, l_scrn.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, p_position, 0);

    // 2 - normal color buffer
    glGenTextures(1, &p_normal);
    glBindTexture(GL_TEXTURE_2D, p_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, l_scrn.x, l_scrn.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, p_normal, 0);

    // 3 - color + specular color buffer
    glGenTextures(1, &p_colorSpec);
    glBindTexture(GL_TEXTURE_2D, p_colorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, l_scrn.x, l_scrn.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, p_colorSpec, 0);

    // 4 - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int l_attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3, l_attachments);

    // 5 - depth buffer
    glGenRenderbuffers(1, &p_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, p_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, l_scrn.x, l_scrn.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, p_depth);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf( "graphic::initDeferredShading framebuffer didnt complete!\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

int graphic::createLight( glm::vec3 pos, glm::vec3 color) {
    light l_light( p_index_light);

    l_light.setPositon( pos);
    l_light.setColor( color);

    p_index_light++;
    return p_index_light-1;
}
