#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <SDL2/SDL.h>

#define TILESET_WIDTH 256
#define TILESET_HEIGHT 256

class Display {
public:
    Display(int width, int height, const std::string& title);
    virtual ~Display();

    void Clear();
    void SwapBuffers();
    void SetTitle( std::string& title);

    void SetSize( int screen_width, int screen_height) {
        Width = screen_width;
        Height = screen_height;
	}

	void SetBackgroundColor( float red = 1.0f, float green= 1.0f, float blue = 1.0f, float alpha = 1.0f) {
	    p_red = red;
	    p_green = green;
	    p_blue = blue;
	    p_alpha = alpha;
	}

    inline int GetWidth() { return Width; }
	inline int GetHeight() { return Height; }

	inline int GetTilesetWidth() { return TILESET_WIDTH; }
	inline int GetTilesetHeight() { return TILESET_HEIGHT; }

	inline int GetBackgroundRed() { return p_red; }
	inline int GetBackgroundGreen() { return p_green; }
	inline int GetBackgroundBlue() { return p_blue; }
	inline int GetBackgroundAlpha() { return p_alpha; }

	inline SDL_Window* GetWindow() { return p_window; }
	inline SDL_GLContext GetContext() { return p_glContext; }
	inline SDL_Renderer* GetRenderer() { return p_renderer; }
	inline SDL_Surface* GetSurface() { return p_surface; }
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
