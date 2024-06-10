#pragma once
#include "SDL/SDL.h"

class Renderer
{
public :
	static SDL_Renderer* GetRenderer();
	static SDL_Window* GetWindow();
private :
	friend class Game;

	static bool Initialize();
	static void Finalize();

	static void PostRender();
	static void PreRender();

	// Renderer for 2D drawing
	static SDL_Renderer* mRenderer;
	// Window created by SDL
	static SDL_Window* mWindow;

};

