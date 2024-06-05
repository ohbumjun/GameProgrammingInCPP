#pragma once

#include "Uuid.h"
#include "BaseObject.h"
#include <cstdint>

//Unity AssetType = https://docs.unity3d.com/Manual/AssetTypes.html
enum class AssetType : char
{
	IMAGE = 0,
	IMAGE_FONT,
	ANIMATION_GRAPH,
	SCENE_GRAPH,
	PREFAB,
	SCENE,
	SHADER,
	MESH,
	FONT,
	TEXT,
	PARTICLE,
	ANIMATION,
	POSTPROCESS,
	SEQUENCER,
	HLSL,
	UNKNOWN, //���� �� (extension���� �͵�)
	NOT_ASSET, //Asset�� �ƴ� ��. �������� �� �� ���� ����ó�� �Ǵ� ���.
	ATLAS,
	SPRITE,
	AUDIO,
	MATERIAL,
	FBX
};
class Asset : public BaseObject
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

	Uuid uuid;
};

