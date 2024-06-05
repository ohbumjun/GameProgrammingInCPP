#include "AssetProcessor.h"

void AssetProcessor::Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath)
{
}

bool AssetProcessor::CanRecord(const std::string& resourcePath)
{
    return false;
}

Asset* AssetProcessor::CreateAsset(const FileId& fileId, const std::string& relativResourcePath, bool isBuiltIn)
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
