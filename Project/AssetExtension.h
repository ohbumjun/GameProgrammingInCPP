#pragma once

#include "Common.h"
#include "AssetAttribute.h"

class AssetExtension
{
	friend class AssetManager;

	static void initialize();
	static void finalize();

	// static std::vector<AssetAttribute*> _attributies;
	static std::unordered_map<size_t, AssetAttribute*> _attributies;
};

