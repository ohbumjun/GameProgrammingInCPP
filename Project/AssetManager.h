#pragma once

#include "Asset.h"
#include "Common.h"
#include "Guid.h"

class AssetManager
{
	// Asset �� ��κ��� guid �� ������ش�.
	// �׸��� �� guid �� key �� �Ͽ�, Asset ���� �����Ѵ�.
	// static GuidManager* guidManager;

	static void ImportAsset(const std::string& relativePath);
};

