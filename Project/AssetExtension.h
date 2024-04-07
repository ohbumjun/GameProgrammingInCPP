#pragma once

#include "Common.h"
#include "AssetAttribute.h"

class AssetExtension
{
public :
	static void Initialize();

private :
	static std::vector<AssetAttribute*> _attributies;
};

