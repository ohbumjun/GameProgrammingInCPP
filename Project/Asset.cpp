#include "Asset.h"
#include "FileId.h"


Asset::Asset(const FileId& fileId, const std::string& resourcePath, AssetType type) :
	fileId(fileId), resourcePath(resourcePath), assetType(type), _prototype(nullptr)
{
}

Asset::~Asset()
{
}

BaseObject* Asset::GetData()
{
	return _prototype;
}

void Asset::SetPrototype(BaseObject* prototype)
{
	_prototype = prototype;
}

BaseObject* Asset::GetPrototype()
{
	return _prototype;
}
