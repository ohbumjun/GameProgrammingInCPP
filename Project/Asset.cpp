#include "Asset.h"
#include "FileId.h"


Asset::Asset(const FileId& fileId, const std::string& resourcePath, AssetType type) :
	fileId(fileId), resourcePath(resourcePath), assetType(type), prototype(nullptr)
{
}

Asset::~Asset()
{
}

BaseObject* Asset::GetData()
{
	return prototype;
}

void Asset::SetPrototype(BaseObject* prototype)
{
	prototype = prototype;
}

BaseObject* Asset::GetPrototype()
{
	return prototype;
}
