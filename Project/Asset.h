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
	UNKNOWN, //폴더 등 (extension없는 것들)
	NOT_ASSET, //Asset이 아닌 것. 엔진에서 쓸 수 없고 예외처리 되는 대상.
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
	*Asset 의 size
	*/
	uint64_t size;

	/**
	*해당 Asset의 info (Unity로 치면 meta file). asset resource의 외부적으로 저장해야하는 값이나 check sum값등을 들고있다.
	*/
	class AssetInfo* info;
	// guid
	// fileId

	Uuid uuid;
};

