#pragma once

#include "Asset.h"
#include <unordered_map>
#include <string>

class GUIDManager
{
	struct GUIDInfo
	{
		std::string path;
		std::string guid;
	};

	//key 를 guid 로 해야 한다.
	std::unordered_map<std::string, GUIDInfo> guidMap;
};

class AssetManager
{
	// Asset 은 경로별로 guid 를 만들어준다.
	// 그리고 그 guid 를 key 로 하여, Asset 들을 관리한다.
	static GUIDManager* guidManager;
};

