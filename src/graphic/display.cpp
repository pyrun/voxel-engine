//#define GLEW_STATIC 1

#include "display.h"
#include <stdio.h>
#include <string>

display::display( config *config) {
    std::string l_title;
    int l_pos_x, l_pos_y;
    int l_glMayor, l_glMinor;
    Uint32 rmask, gmask, bmask, amask;

    // set values
    l_title = config->get( "title", "graphic", "selur");
    p_width = atoi( config->get( "width", "graphic", "640").c_str());
    p_height = atoi( config->get( "height", "graphic", "400").c_str());
    l_pos_x = atoi( config->get( "window_x", "graphic", std::to_string(SDL_WINDOWPOS_CENTERED) ).c_str());
    l_pos_y = atoi( config->get( "window_y", "graphic", std::to_string(SDL_WINDOWPOS_CENTERED) ).c_str());

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
    // This line is only needed, if you want debug the program
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

    // disable V-Sync for openVR support( need more then 60Hz)
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");

    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "display::display failed to start the SDL system\n");
        return;
    }

	// minimum version of opengl
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // which opengl context we use
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    // disable multisamplebuffers
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 0);

    // size of color channel
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 3);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 3);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 2);
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 1);

    // size of the depth channel
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);

    // we use double buffer
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);

    // creating window with the opengl context
	p_window = SDL_CreateWindow( l_title.c_str(), l_pos_x, l_pos_y, p_width, p_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if( p_window == NULL) {
        printf( "cant create the window %dx%d %dw %dh\n", l_pos_x, l_pos_y, p_width, p_height);
        return;
	}
	p_glContext = (SDL_GLContext *)SDL_GL_CreateContext(p_window);
	if( p_glContext == NULL){
        printf( "display::display cant create opengl context\n");
        SDL_DestroyWindow( p_window);
        return;
	}
    p_surface = SDL_CreateRGBSurface( 0, getTilesetWidth(), getTilesetHeight(), 32, rmask, gmask, bmask, amask);
    if( p_surface == NULL) {
        printf( "display::display software surface cant create; error \"%s\"\n", SDL_GetError());
    }
    p_renderer = SDL_CreateRenderer( p_window, -1, SDL_RENDERER_ACCELERATED);
    if( p_renderer == NULL) {
        printf( "display::display renderer could not be created; error \"%s\"", SDL_GetError());
    }

	// initializing glew
	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
    if(res != GLEW_OK) {
		printf( "display::display glew was failed to initialize\n" );
    }
    printf("display::display engine use opengl platform version \"%s\"\n",glGetString(GL_VERSION));

    // disable the build in vsync
    SDL_GL_SetSwapInterval(0);

    // depth test enable for z sorting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // cull face out that we not see
	glEnable( GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);

	// enable blend for transparency
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// clear background
	setBackgroundColor();
}

display::~display() {
    SDL_GL_DeleteContext( p_glContext);
	SDL_DestroyWindow( p_window);
	SDL_FreeSurface( p_surface);
	SDL_Quit();
}

void display::clear( bool black) {
    if( black) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        p_backgroundcolor_change = true;
    } else if( p_backgroundcolor_change) {
        glClearColor( p_red, p_green, p_blue, p_alpha);
        p_backgroundcolor_change = false;
    }
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glViewport( 0, 0, getWidth(), getHeight());
}

void display::swapBuffers() {
	SDL_GL_SwapWindow( p_window);
}

void display::setTitle( std::string& title) {
    SDL_SetWindowTitle( p_window, title.c_str());
}
