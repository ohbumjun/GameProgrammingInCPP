#pragma once

#include "AssetProcessor.h"

class SpriteAssetProcessor : public AssetProcessor
{
public:
	// virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false);
	virtual Asset* CreateAsset(const FileId& uuid, const std::string& resourcePath, bool isBuiltIn = false);

	virtual void WriteResourceFile(Asset& asset) {}

	virtual void WriteCacheFile(Asset& asset){}
};

