#pragma once

#include "Asset.h"
#include "Common.h"
#include "Guid.h"

class AssetManager
{
	// Asset 은 경로별로 guid 를 만들어준다.
	// 그리고 그 guid 를 key 로 하여, Asset 들을 관리한다.
	// static GuidManager* guidManager;

	static void ImportAsset(const std::string& relativePath);
};

