//#define GLEW_STATIC 1

#include "display.h"
#include <GL/glew.h>
#include <stdio.h>

Display::Display(int width, int height, const std::string& title) {
    Width = width;
    Height = height;
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Could not init SDL");
        return;
    }
	// benötigt opengl 4.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

    // bit grße der Farben
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 3);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 3);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 2);
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 1);
    // Tiefenbuffergröße
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 1);
    // Doublebuffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 2);

    // SDL Init
    // Erstelle Fenster
	m_window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if( m_window == NULL)
        printf( "error\n");
	m_glContext = SDL_GL_CreateContext(m_window);
	if( m_glContext == NULL)
        printf( "error\n");
    m_surface = SDL_CreateRGBSurface( 0, GetTilesetWidth(), GetTilesetHeight(), 32, rmask, gmask, bmask, amask);
    if( m_surface == NULL) {
        printf( "Display: Surface cant create %s\n", SDL_GetError());
    }
    // Renderer
    m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED);
    if( m_renderer == NULL) {
        printf( "Display: Renderer could not be created! SDL Error \"%s\"", SDL_GetError());
    }
	// Glew starten
	GLenum res = glewInit();
    if(res != GLEW_OK) {
		printf( "Glew failed to initialize!" );
    }
    printf("OpenGL version supported by this platform (%s): \n",glGetString(GL_VERSION));

    int Depth;
    SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &Depth);
    printf( "%d Depth\n", Depth);
    // Tiefe nützen
    glEnable( GL_DEPTH_TEST);

    // Cull Face
	glEnable( GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);

	// Blend
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// background color
	SetBackgroundColor();
}

Display::~Display() {
    SDL_GL_DeleteContext( m_glContext);
	SDL_DestroyWindow( m_window);
	SDL_FreeSurface( m_surface);
	SDL_Quit();
}

void Display::Clear() {
    // setzte background color und clear BUFFER
    glClearColor( m_red, m_green, m_blue, m_alpha);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
}

void Display::SwapBuffers() {
	SDL_GL_SwapWindow( m_window);
}

void Display::SetTitle( std::string& title) {
    SDL_SetWindowTitle( m_window, title.c_str());
}
