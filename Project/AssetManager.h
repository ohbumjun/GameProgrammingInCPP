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

	//key �� guid �� �ؾ� �Ѵ�.
	std::unordered_map<std::string, GUIDInfo> guidMap;
};

class AssetManager
{
	// Asset �� ��κ��� guid �� ������ش�.
	// �׸��� �� guid �� key �� �Ͽ�, Asset ���� �����Ѵ�.
	static GUIDManager* guidManager;
};

