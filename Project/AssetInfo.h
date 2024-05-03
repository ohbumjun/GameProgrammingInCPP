#pragma once

#include "Asset.h"
#include <string>
#include <cstdint>

class AssetInfo
{
public:

	// struct SubAssetInfo : LvArchivable
	struct SubAssetInfo
	{
		// void Serialize(Engine::LvArchive & archive) override;
		// void Deserialize(Engine::LvArchive& archive) override;

		std::string name;
		uint32_t localId = 0;
		uint8_t assetType = 0;
	};
	friend class Asset;

	AssetInfo() = default;
	AssetInfo(const AssetInfo& rhs) = default;
	AssetInfo& operator=(const AssetInfo& rhs) = default;


	virtual void CopyFrom(const AssetInfo* info)
	{
		// NOTICE �ڽ� ��ü�� ���� �θ�Ŭ���� �����ͷ� ������Կ��� �����(*base = *obj) Base�� operator=�� ����Ǿ� ����� ���簡 �ȵȴ�.
		// Base* base1 = new Derived();
		// Base* base2 = new Derived();
		// *base1 = *base2;
		// ���� ���� ��Ȳ���� Derived�� operaotr=�� ȣ���ϱ� ���� 2���� ����� �ִ�.
		// 1. virtual void CopyFrom(Base* rhs) �����Ͽ� override
		// 2. virtual Base& operator=(Base& rhs) �����Ͽ� �ڽİ�ü�� ���� �ñ״����� �޼��带 override
		// ���⼭ 1���� ����� ����Ͽ� �����Ѵ�. (TODO operator=�� �ϴ� ���� ���� ������ ���Ŀ� �������� ���� �����ʿ�)

		//LvObject�� ���Ѱ� �������� �ʴ´�. Why? LvObject::Instantiate�� �� �� �־��ֱ� ������. ������ ���ú����� ����� ����������.
		version = info->version;

		// guid�� �������� �ʴ´�!
		//guid = info->guid; 
	}

	// TODO ��Ʈ �������� �ٲ���.
	enum class State : uint8_t
	{
		NONE = 0,
		FILE_NOT_EXIST, // .Inf�� �������� ����.
		GUID_NOT_MATCH,
		CHECKSUM_NOT_MATCH
	};

	constexpr static const char* FileExtension = ".inf";

	/**
	* asset info �� serialize ����
	*/
	std::string version;

	/**
	* ����� asset�� guid
	*/
	std::string guid;

	/**
	*AssetInfo�� �����մϴ�.
	* type �����Ϸ��� AssetInfo Type
	* @param path asset info�� ��ġ
	*/
	// static AssetInfo* Create(LvTypeId type, const std::string& path);

	/**
	* asset�� �ش��ϴ� .inf�� ����ϴ�.
	* asset .inf�� ������ target asset
	* @return .Inf ���� �������θ� ��ȯ�մϴ�.
	*/
	inline static bool Save(const Asset* asset)
	{
		// if (!(asset != nullptr && asset->info != nullptr))
		// {
		// 	return false;
		// }
		// return SaveFile(asset);
	}

};

