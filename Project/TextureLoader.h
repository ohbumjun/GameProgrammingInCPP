#pragma once

#include "Common.h"
#include "SDL/SDL_image.h"

class TextureLoader
{
public :
	static SDL_Texture* LoadTexture(const std::string& relativePath);
};

