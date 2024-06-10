#pragma once

#include "Asset.h"

class ImageAsset : public Asset
{
public :
	ImageAsset(const FileId& fileId, const std::string& resourcePath);
};


