#pragma once
#include "SDL/SDL.h"

class SDL_Renderer;
class SDL_Window;

class Renderer
{
public :
	static SDL_Renderer* GetRenderer();
	static SDL_Window* GetWindow();
private :
	friend class Game;

	static bool initialize();
	static void finalize();

	static void postRender();
	static void preRender();

	// Renderer for 2D drawing
	static SDL_Renderer* mRenderer;
	// Window created by SDL
	static SDL_Window* mWindow;

};

