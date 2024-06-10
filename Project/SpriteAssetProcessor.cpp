#include "SpriteAssetProcessor.h"
#include "Filesystem.h"
#include "SpriteAsset.h"

Asset* SpriteAssetProcessor::CreateAsset(const FileId& uuid, const std::string& resourcePath, bool isBuiltIn)
{
	const std::string& name = FileSystem::GetPathName(resourcePath.c_str());

	// SpriteAsset* asset = &LvObject::Instantiate<LvMaterialAsset, const LvUuid&, const LvString&>(name, uuid, resourcePath, isBuiltIn);
	// SpriteAsset* asset = new SpriteAsset();
	SpriteAsset* asset = nullptr;

	// writeVersion(asset);

	return asset;
}
