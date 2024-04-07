#include "TextureLoader.h"
#include "Renderer.h"

SDL_Texture* TextureLoader::LoadTexture(const std::string& relativePath)
{
	SDL_Texture* tex = nullptr;
	
	// Load from file
	// Image 를 load 하여 SDL_Surface 안에 담는다.
	SDL_Surface* surf = IMG_Load(relativePath.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", relativePath.c_str());
		return nullptr;
	}

	// Create texture from surface (SDL_Surface 를 SDL_Texture 로 변환)
	// SDL_Texture 가 drawing 을 위해 필요한 것이다.

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
