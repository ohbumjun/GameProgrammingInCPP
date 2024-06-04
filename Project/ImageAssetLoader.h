#pragma once

#include "AssetProcessor.h"
#include "Asset.h"
#include "Uuid.h"
#include <string>

class ImageAssetLoader : public AssetProcessor
{
	// Asset * CreateAsset(const Uuid & uuid, const std::string & resourcePath, bool isBuiltIn = false) override;
	Asset * CreateAsset(const Uuid& uuid, const std::string & resourcePath, bool isBuiltIn = false);

};

