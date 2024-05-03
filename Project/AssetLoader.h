#pragma once

#include "Asset.h"
#include <string>
#include "Uuid.h"

class AssetLoader
{
	/**
	* @brief Asset�� Import�Ѵ�.
	* @details Resource���Ϸκ��� Cache������ �����Ѵ�.
	* @param asset uuid�� type�� ���õ� asset
	*/
	virtual void Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath);

	/**
	* @brief : Asset validate before record.
	*/
	virtual bool CanRecord(const std::string& resourcePath);

	/**
	* @brief : editor���� ����� LvAsset data�� �����մϴ�.
	*/
	virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false) = 0;

	/**
	* @brief reosurce ������ ���ϴ�.
	*/
	virtual void WriteResourceFile(Asset& asset) = 0;

	/**
	* @brief asset �� cache ������ ���ϴ�.
	*/
	virtual void WriteCacheFile(Asset& asset) = 0;

	/**
	* @brief info�� �ִ� �����͸� ��򰡿� ���� ���� �ʿ䰡 �ִٸ� ���.
	*/
	virtual void WriteInfo(Asset* asset);

};

