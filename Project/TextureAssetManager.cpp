#include "TextureAssetManager.h"
#include "TextureAssetProcessor.h"
#include "SDL/SDL_image.h"

std::unordered_map<std::string, SDL_Texture*> TextureAssetManager::mTextures;

SDL_Texture* TextureAssetManager::loadTexture(const std::string& relativePath)
{
	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(relativePath);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		// Image 를 load 하여 SDL_Surface 안에 담는다.
		tex = TextureAssetProcessor::LoadTexture(relativePath);

		
	}

	if (tex)
	{
		mTextures.emplace(relativePath.c_str(), tex);
	}

	return tex;
}

void TextureAssetManager::finalize()
{
	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

void TextureAssetManager::initialize()
{
	// 일단 여기서 임시로 로드한다.
	loadTexture("Assets/Ship.png");
	loadTexture("Assets/Asteroid.png");
	loadTexture("Assets/Laser.png");
}

SDL_Texture* TextureAssetManager::getTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;

	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);

	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	
	return tex;
}
