#pragma once

#include "AssetProcessor.h"

class ImageAssetProcessor : public AssetProcessor
{
public:
	// virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false);
	virtual Asset* CreateAsset(const FileId& uuid, const std::string& resourcePath, bool isBuiltIn = false);

	virtual void DestroyAsset(Asset* asset);

	virtual void onLoad(Asset* asset, const std::string& resAbsolutePath) override;

	virtual void WriteResourceFile(Asset& asset) {}

	virtual void WriteCacheFile(Asset& asset) {}
};

