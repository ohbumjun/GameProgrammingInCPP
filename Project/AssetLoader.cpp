#include "AssetLoader.h"

void AssetLoader::Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath)
{
}

bool AssetLoader::CanRecord(const std::string& resourcePath)
{
    return false;
}

Asset* AssetLoader::CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn)
{
    return nullptr;
}

void AssetLoader::WriteResourceFile(Asset& asset)
{
}

void AssetLoader::WriteCacheFile(Asset& asset)
{
}

void AssetLoader::WriteInfo(Asset* asset)
{
}
