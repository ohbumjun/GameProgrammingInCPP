#pragma once

#include "BaseObject.h"
#include <cstdint>

//Unity AssetType = https://docs.unity3d.com/Manual/AssetTypes.html
enum class AssetType : char
{
	IMAGE = 0,
	SHADER,
	MESH,
	ANIMATION,
	AUDIO,
	MATERIAL,
	FBX,
	FONT,
	IMAGE_FONT,
	ANIMATION_GRAPH,
	PREFAB,
	SCENE,
	TEXT,
	PARTICLE,
	ATLAS,
	SPRITE,
	SCENE_GRAPH,
	POSTPROCESS,
	SEQUENCER,
	HLSL,
	UNKNOWN, //���� �� (extension���� �͵�)
	NOT_ASSET //Asset�� �ƴ� ��. �������� �� �� ���� ����ó�� �Ǵ� ���.
};
class Asset
{	
public :

	Asset();
	~Asset();

	BaseObject* GetData();
	void SetPrototype(BaseObject* prototype);
	BaseObject* GetPrototype();
private :
	BaseObject* prototype;
	AssetType assetType;
	// guid
	// fileId
};

