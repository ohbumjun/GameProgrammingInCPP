#pragma once

#include "FileId.h"
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
	friend class AssetManager;
public :
	Asset(const FileId& fileId, const std::string& resourcePath, AssetType type);
	~Asset();

	BaseObject* GetData();
	void SetPrototype(BaseObject* prototype);
	BaseObject* GetPrototype();
	const std::string& GetResourcePath() { return resourcePath; }
	AssetType GetAssetType() {return assetType; }
protected :
	virtual void onCreate() {};
private :
	BaseObject* _prototype;
	AssetType assetType;
	std::string resourcePath;
	FileId fileId;
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

