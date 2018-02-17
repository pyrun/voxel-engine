#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <SDL2/SDL.h>

#define TILESET_WIDTH 256
#define TILESET_HEIGHT 256

class display {
public:
    display(int width, int height, const std::string& title);
    virtual ~display();

    void clear();
    void swapBuffers();
    void setTitle( std::string& title);

    void setSize( int screen_width, int screen_height) {
        Width = screen_width;
        Height = screen_height;
	}

	void setBackgroundColor( float red = 1.0f, float green= 1.0f, float blue = 1.0f, float alpha = 1.0f) {
	    p_red = red;
	    p_green = green;
	    p_blue = blue;
	    p_alpha = alpha;
	}

    inline int getWidth() { return Width; }
	inline int getHeight() { return Height; }

	inline int getTilesetWidth() { return TILESET_WIDTH; }
	inline int getTilesetHeight() { return TILESET_HEIGHT; }

	inline int getBackgroundRed() { return p_red; }
	inline int getBackgroundGreen() { return p_green; }
	inline int getBackgroundBlue() { return p_blue; }
	inline int getBackgroundAlpha() { return p_alpha; }

	inline SDL_Window* getWindow() { return p_window; }
	inline SDL_GLContext getContext() { return p_glContext; }
	inline SDL_Renderer* getRenderer() { return p_renderer; }
	inline SDL_Surface* getSurface() { return p_surface; }
protected:
private:
    int Width;
	int Height;
	SDL_Window* p_window;
	SDL_GLContext p_glContext;
	SDL_Renderer* p_renderer;
	SDL_Surface* p_surface;
	float p_red;
	float p_green;
	float p_blue;
	float p_alpha;
};

#endif // DISPLAY_H
