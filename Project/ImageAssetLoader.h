#pragma once

#include "AssetLoader.h"
#include "Asset.h"
#include "Uuid.h"
#include <string>

class ImageAssetLoader : public AssetLoader
{
	// Asset * CreateAsset(const Uuid & uuid, const std::string & resourcePath, bool isBuiltIn = false) override;
	Asset * CreateAsset(const Uuid& uuid, const std::string & resourcePath, bool isBuiltIn = false);

};

