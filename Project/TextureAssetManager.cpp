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
		// Image �� load �Ͽ� SDL_Surface �ȿ� ��´�.
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
	// �ϴ� ���⼭ �ӽ÷� �ε��Ѵ�.
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
