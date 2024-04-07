#pragma once

#include "Common.h"
#include "SDL/SDL.h"

class SDL_Texture;

class TextureAssetManager
{
	friend class AssetManager;

	static SDL_Texture* loadTexture(const std::string& relativePath);
	static SDL_Texture* getTexture(const std::string& fileName);
	static void finalize();
	static void initialize();

	// Map of textures loaded
	static std::unordered_map<std::string, SDL_Texture*> mTextures;
};

