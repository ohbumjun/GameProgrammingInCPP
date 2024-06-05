#include "ImageAssetProcessor.h"
#include "Filesystem.h"
#include "ImageAsset.h"

Asset* ImageAssetProcessor::CreateAsset(const FileId& uuid, const std::string& resourcePath, bool isBuiltIn)
{
	const std::string& name = FileSystem::GetPathName(resourcePath.c_str());

	// SpriteAsset* asset = &LvObject::Instantiate<LvMaterialAsset, const LvUuid&, const LvString&>(name, uuid, resourcePath, isBuiltIn);
	ImageAsset* asset = new ImageAsset();

	// writeVersion(asset);

	return asset;
}
