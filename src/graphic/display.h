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
	    m_red = red;
	    m_green = green;
	    m_blue = blue;
	    m_alpha = alpha;
	}

    inline int GetWidth() { return Width; }
	inline int GetHeight() { return Height; }

	inline int GetTilesetWidth() { return TILESET_WIDTH; }
	inline int GetTilesetHeight() { return TILESET_HEIGHT; }

	inline int GetBackgroundRed() { return m_red; }
	inline int GetBackgroundGreen() { return m_green; }
	inline int GetBackgroundBlue() { return m_blue; }
	inline int GetBackgroundAlpha() { return m_alpha; }

	inline SDL_Window* GetWindow() { return m_window; }
	inline SDL_GLContext GetContext() { return m_glContext; }
	inline SDL_Renderer* GetRenderer() { return m_renderer; }
	inline SDL_Surface* GetSurface() { return m_surface; }
protected:
private:
    int Width;
	int Height;
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	SDL_Renderer* m_renderer;
	SDL_Surface* m_surface;
	float m_red;
	float m_green;
	float m_blue;
	float m_alpha;
};

#endif // DISPLAY_H
