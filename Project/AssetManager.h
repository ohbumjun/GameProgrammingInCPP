#pragma once

#include "Asset.h"
#include "Common.h"
#include "Guid.h"
#include "TextureAssetManager.h"

class SDL_Texture;

class AssetManager
{
	friend class Game;

	// Asset 은 경로별로 guid 를 만들어준다.
	// 그리고 그 guid 를 key 로 하여, Asset 들을 관리한다.
	// static GuidManager* guidManager;
public :
	static void ImportAsset(const std::string& relativePath);

	// Texture
	static SDL_Texture* LoadTexture(const std::string& relativePath);
	static SDL_Texture* GetTexture(const std::string& fileName);

private :
	static void initialize();
	static void finalize();
};

