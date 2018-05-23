#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../engine/config.h"

#define TILESET_WIDTH 1024
#define TILESET_HEIGHT 1024

class display {
public:
    display( config *config);
    virtual ~display();

    void clear( bool black = false);
    void swapBuffers();
    void setTitle( std::string& title);

    void setSize( int screen_width, int screen_height) {
        p_width = screen_width;
        p_height = screen_height;
	}

	void setSize( glm::vec2 size) {
        p_width = size.x;
        p_height = size.y;
	}

	void setBackgroundColor( float red = 1.0f, float green= 1.0f, float blue = 1.0f, float alpha = 1.0f) {
	    p_red = red;
	    p_green = green;
	    p_blue = blue;
	    p_alpha = alpha;
	    p_backgroundcolor_change = true;
	}

    inline int getWidth() { return p_width; }
	inline int getHeight() { return p_height; }
	inline glm::vec2 getSize() { return glm::vec2( p_width, p_height); }

	inline int getTilesetWidth() { return TILESET_WIDTH; }
	inline int getTilesetHeight() { return TILESET_HEIGHT; }

	inline int getBackgroundRed() { return p_red; }
	inline int getBackgroundGreen() { return p_green; }
	inline int getBackgroundBlue() { return p_blue; }
	inline int getBackgroundAlpha() { return p_alpha; }

	inline SDL_Window* getWindow() { return p_window; }
	inline SDL_GLContext* getContext() { return p_glContext; }
	inline SDL_Renderer* getRenderer() { return p_renderer; }
	inline SDL_Surface* getSurface() { return p_surface; }
protected:
private:
    int p_width;
	int p_height;
	SDL_Window* p_window;
	SDL_GLContext* p_glContext;
	SDL_Renderer* p_renderer;
	SDL_Surface* p_surface;
	float p_red;
	float p_green;
	float p_blue;
	float p_alpha;
	bool p_backgroundcolor_change;
};

#endif // DISPLAY_H
