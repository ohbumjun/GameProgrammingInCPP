#pragma once

#include "Common.h"
#include "AssetProcessor.h"
#include "SDL/SDL_image.h"

class TextureAssetProcessor : public AssetProcessor
{
public :
	static SDL_Texture* LoadTexture(const std::string& relativePath);
};

