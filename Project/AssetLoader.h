#pragma once

#include "Asset.h"
#include <string>
#include "Uuid.h"

class AssetLoader
{
	/**
	* @brief Asset을 Import한다.
	* @details Resource파일로부터 Cache파일을 생성한다.
	* @param asset uuid와 type이 셋팅된 asset
	*/
	virtual void Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath);

	/**
	* @brief : Asset validate before record.
	*/
	virtual bool CanRecord(const std::string& resourcePath);

	/**
	* @brief : editor에서 사용할 LvAsset data를 생성합니다.
	*/
	virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false) = 0;

	/**
	* @brief reosurce 파일을 씁니다.
	*/
	virtual void WriteResourceFile(Asset& asset) = 0;

	/**
	* @brief asset 의 cache 파일을 씁니다.
	*/
	virtual void WriteCacheFile(Asset& asset) = 0;

	/**
	* @brief info에 있는 데이터를 어딘가에 셋팅 해줄 필요가 있다면 사용.
	*/
	virtual void WriteInfo(Asset* asset);

};

