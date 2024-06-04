#include "AssetProcessor.h"

void AssetProcessor::Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath)
{
}

bool AssetProcessor::CanRecord(const std::string& resourcePath)
{
    return false;
}

Asset* AssetProcessor::CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn)
{
    return nullptr;
}

// void AssetProcessor::WriteResourceFile(Asset& asset)
// {
// }
// 
// void AssetProcessor::WriteCacheFile(Asset& asset)
// {
// }
// 
// void AssetProcessor::WriteInfo(Asset* asset)
// {
// }
