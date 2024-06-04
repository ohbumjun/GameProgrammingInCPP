#pragma once

#include "AssetProcessor.h"

class SpriteAssetProcessor : public AssetProcessor
{
public:
	SpriteAssetProcessor() {};
	virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false);
};

