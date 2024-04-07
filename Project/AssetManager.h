#pragma once

#include "Asset.h"
#include "Common.h"
#include "Guid.h"
#include "TextureAssetManager.h"

class SDL_Texture;

class AssetManager
{
	friend class Game;

	// Asset �� ��κ��� guid �� ������ش�.
	// �׸��� �� guid �� key �� �Ͽ�, Asset ���� �����Ѵ�.
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

