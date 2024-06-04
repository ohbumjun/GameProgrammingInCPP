#include "AssetManager.h"
#include "AssetExtension.h"
#include "SpriteAssetProcessor.h"


std::unordered_map<AssetType, AssetProcessor*> AssetManager::_loaders;

void AssetManager::initializeProcessors()
{
	_loaders[AssetType::SPRITE] = new SpriteAssetProcessor();
}

void AssetManager::initialize()
{
	AssetExtension::initialize();

	TextureAssetManager::initialize();
}

void AssetManager::finalize()
{
	AssetExtension::finalize();

	TextureAssetManager::finalize();

	// 각 type 별로 Asset Record 객체도 할당해준다.
}

void AssetManager::createAsset()
{
}

SDL_Texture* AssetManager::LoadTexture(const std::string& relativePath)
{
	return TextureAssetManager::loadTexture(relativePath);
}

SDL_Texture* AssetManager::GetTexture(const std::string& fileName)
{
	return TextureAssetManager::getTexture(fileName);
}

Asset* AssetManager::CreateAsset(AssetType type, const std::string& path)
{
	return nullptr;
}

void AssetManager::LoadAsset(const std::string& relativePath)
{
	// extension 을 통해 asset type 추출
	AssetType assetType = AssetExtension::GetAssetTypeByExtension(relativePath);

	// 해당 asset type 으로 asset record 객체 가져오기
	AssetProcessor* assetProcessor = _loaders[assetType];

	// "relativePath" 로 asset 가져오기
	// 없으면 이제 Load
	// 이때 guid 랑 uuid 도 발급해주기
	// 해당 정보로 CreateAsset 호출
	// Asset Record 가져와서, AssetRecord 의 CreateAsset 함수 호출
	// (여기서부터는 나의 의지..?) 그리고 실제 prototype load 도 수행하기.
}

void AssetManager::ImportAsset(const std::string& relativePath)
{
}
