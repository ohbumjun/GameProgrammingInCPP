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
	/**
	*Asset �� size
	*/
	uint64_t size;

	/**
	*�ش� Asset�� info (Unity�� ġ�� meta file). asset resource�� �ܺ������� �����ؾ��ϴ� ���̳� check sum������ ����ִ�.
	*/
	class AssetInfo* info;
	// guid
	// fileId
};

