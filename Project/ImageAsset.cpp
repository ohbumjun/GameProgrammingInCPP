#include "ImageAsset.h"

ImageAsset::ImageAsset(const FileId& fileId, const std::string& resourcePath) :
	Asset(fileId, resourcePath, AssetType::IMAGE)
{
}
