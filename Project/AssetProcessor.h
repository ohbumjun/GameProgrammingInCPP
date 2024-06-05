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
	* @brief Asset을 Import한다.
	* @details Resource파일로부터 Cache파일을 생성한다.
	* @param asset uuid와 type이 셋팅된 asset
	* 
	* 개선 방향
	* - 이 녀석은 Resource 파일에서 Load 만 하기
	* - Cache 로 만드는 과정은 다른 함수를 통해
	* - 그리고 전체 관리 Table 에 추가하는 함수도 별도로 뚫어두기
	*/
	virtual void Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath);

	
	virtual bool CanRecord(const std::string& resourcePath);

	
	// virtual Asset* CreateAsset(const Uuid& uuid, const std::string& resourcePath, bool isBuiltIn = false) = 0;
	virtual Asset* CreateAsset(const FileId& uuid, const std::string& relativResourcePath, bool isBuiltIn = false) = 0;

	// /**
	// * @brief reosurce 파일을 씁니다.
	// */
	// virtual void WriteResourceFile(Asset& asset) = 0;
	// 
	// /**
	// * @brief asset 의 cache 파일을 씁니다.
	// */
	// virtual void WriteCacheFile(Asset& asset) = 0;
	// 
	// /**
	// * @brief info에 있는 데이터를 어딘가에 셋팅 해줄 필요가 있다면 사용.
	// */
	// virtual void WriteInfo(Asset* asset);

	AssetType m_AssetType;
};

