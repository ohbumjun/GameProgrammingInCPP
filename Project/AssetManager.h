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

	// Asset �� ��κ��� guid �� ������ش�.
	// �׸��� �� guid �� key �� �Ͽ�, Asset ���� �����Ѵ�.
	// static GuidManager* guidManager;
public :
	// �̸� �ʿ��� ��� asset ���ϵ��� Load �ϰ�, Cache ���Ϸ� �����ϴ� �Լ�
	static void LoadAsset(const std::string& relativePath);
	static void ImportAsset(const std::string& relativePath);
	static void PrepareAssets();
	static void Initialize();
	static void Finalize();
	static void CreateAsset();

	
	// Texture
	static SDL_Texture* LoadTexture(const std::string& relativePath);
	static SDL_Texture* GetTexture(const std::string& fileName);

	static Asset* CreateAsset(AssetType type, const std::string& path);

	static std::string GetAbsoluteResourcePath(const std::string& relativeResourcePath);
private :
	
	static void initializeProcessors();
	
	static std::unordered_map<AssetType, AssetProcessor*> _loaders;
	static std::unordered_map<std::string/*Resources/~~ ��� ����� ?*/, Asset*> _assets;
};

