#include "TextureLoader.h"
#include "Renderer.h"

SDL_Texture* TextureLoader::LoadTexture(const std::string& relativePath)
{
	SDL_Texture* tex = nullptr;
	
	// Load from file
	// Image �� load �Ͽ� SDL_Surface �ȿ� ��´�.
	SDL_Surface* surf = IMG_Load(relativePath.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", relativePath.c_str());
		return nullptr;
	}

	// Create texture from surface (SDL_Surface �� SDL_Texture �� ��ȯ)
	// SDL_Texture �� drawing �� ���� �ʿ��� ���̴�.

	SDL_Renderer* renderer = Renderer::GetRenderer();
	
	tex = SDL_CreateTextureFromSurface(renderer, surf);
	
	SDL_FreeSurface(surf);
	
	if (!tex)
	{
		SDL_Log("Failed to convert surface to texture for %s", relativePath.c_str());
		return nullptr;
	}

	return tex;
}
