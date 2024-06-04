#pragma once

#include "Asset.h"
#include "Common.h"
#include "Guid.h"
#include "TextureAssetManager.h"

class AssetProcessor;
struct SDL_Texture;

class AssetManager
{
	friend class Game;

	// Asset 은 경로별로 guid 를 만들어준다.
	// 그리고 그 guid 를 key 로 하여, Asset 들을 관리한다.
	// static GuidManager* guidManager;
public :
	static void LoadAsset(const std::string& relativePath);
	static void ImportAsset(const std::string& relativePath);
	
	// Texture
	static SDL_Texture* LoadTexture(const std::string& relativePath);
	static SDL_Texture* GetTexture(const std::string& fileName);

	static Asset* CreateAsset(AssetType type, const std::string& path);

private :
	static void initialize();
	static void initializeProcessors();
	static void finalize();
	static void createAsset();

	static std::unordered_map<AssetType, AssetProcessor*> _loaders;
};

