#pragma once

#include "Asset.h"
#include <string>
#include "Uuid.h"
#include "FileId.h"

class AssetProcessor
{
	friend class AssetManager;

protected:

	/**
	* @brief Asset�� Import�Ѵ�.
	* @details Resource���Ϸκ��� Cache������ �����Ѵ�.
	* @param asset uuid�� type�� ���õ� asset
	* 
	* ���� ����
	* - �� �༮�� Resource ���Ͽ��� Load �� �ϱ�
	* - Cache �� ����� ������ �ٸ� �Լ��� ����
	* - �׸��� ��ü ���� Table �� �߰��ϴ� �Լ��� ������ �վ�α�
	*/
	virtual void Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath);

	
	virtual bool CanRecord(const std::string& resourcePath);

	
	// virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false) = 0;
	virtual Asset* CreateAsset(const FileId& uuid, const std::string& relativResourcePath, bool isBuiltIn = false) = 0;

	// /**
	// * @brief reosurce ������ ���ϴ�.
	// */
	// virtual void WriteResourceFile(Asset& asset) = 0;
	// 
	// /**
	// * @brief asset �� cache ������ ���ϴ�.
	// */
	// virtual void WriteCacheFile(Asset& asset) = 0;
	// 
	// /**
	// * @brief info�� �ִ� �����͸� ��򰡿� ���� ���� �ʿ䰡 �ִٸ� ���.
	// */
	// virtual void WriteInfo(Asset* asset);

	AssetType m_AssetType;
};

