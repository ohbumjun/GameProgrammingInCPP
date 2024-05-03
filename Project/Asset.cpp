#include "Asset.h"

Asset::Asset()
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
