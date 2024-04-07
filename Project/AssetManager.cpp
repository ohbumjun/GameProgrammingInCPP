#include "AssetManager.h"
#include "AssetExtension.h"

void AssetManager::initialize()
{
	AssetExtension::initialize();

	TextureAssetManager::initialize();
}

void AssetManager::finalize()
{
	AssetExtension::finalize();

	TextureAssetManager::finalize();
}

SDL_Texture* AssetManager::LoadTexture(const std::string& relativePath)
{
	return TextureAssetManager::loadTexture(relativePath);
}

SDL_Texture* AssetManager::GetTexture(const std::string& fileName)
{
	return TextureAssetManager::getTexture(fileName);
}

void AssetManager::ImportAsset(const std::string& relativePath)
{
}
