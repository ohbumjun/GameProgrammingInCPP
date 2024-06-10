#include "AssetManager.h"
#include "AssetExtension.h"
#include "SpriteAssetProcessor.h"
#include "ImageAssetProcessor.h"
#include "EditorContext.h"
#include "FileSystem.h"
#include "FileManager.h"

std::unordered_map<AssetType, AssetProcessor*> AssetManager::_loaders;

void AssetManager::initializeProcessors()
{
	_loaders[AssetType::SPRITE] = new SpriteAssetProcessor();
	_loaders[AssetType::IMAGE]	= new ImageAssetProcessor();
}

void AssetManager::PrepareAssets()
{
	// 미리 필요한 파일들을 일단 로드한다.

	// 그리고 Cache 파일로 저장한다.

	LoadAsset("Assets/Ship.png");
	LoadAsset("Assets/Asteroid.png");
	LoadAsset("Assets/Laser.png");
}

void AssetManager::Initialize()
{
	AssetExtension::initialize();

	initializeProcessors();

	TextureAssetManager::initialize();
}

void AssetManager::Finalize()
{
	AssetExtension::finalize();

	TextureAssetManager::finalize();

	// 각 type 별로 Asset Record 객체도 해제해준다.
}

void AssetManager::CreateAsset()
{
}

SDL_Texture* AssetManager::LoadTexture(const std::string& relativePath)
{
	return TextureAssetManager::loadTexture(relativePath);
}

SDL_Texture* AssetManager::GetTexture(const std::string& fileName)
{
	return TextureAssetManager::getTexture(fileName);
}

Asset* AssetManager::CreateAsset(AssetType type, const std::string& path)
{
	return nullptr;
}

std::string AssetManager::GetAbsoluteResourcePath(const std::string& relativeResourcePath)
{
	// LV_CHECK(!LvString::IsNullOrEmpty(relativeResourcePath), "relativeResourcePath is null or empty");

	// if (relativeResourcePath.StartsWith(LvEditorContext::Directories::builtinresource))
	// {
	// 	return LvFileManager::ToAbsolutePath(relativeResourcePath.c_str());
	// }

	return FileManager::ToAbsolutePath(FileSystem::CombinePath(EditorContext::Directories::resources, relativeResourcePath.c_str()).c_str());

}

void AssetManager::LoadAsset(const std::string& relativePath)
{
	// extension 을 통해 asset type 추출
	AssetType assetType = AssetExtension::GetAssetTypeByExtension(relativePath);

	// "relativePath" 로 asset 가져오기


	// 해당 asset type 으로 asset record 객체 가져오기
	AssetProcessor* assetProcessor = _loaders[assetType];

	// 없으면 이제 Load
	// 이때 guid 랑 uuid 도 발급해주기
	// 해당 정보로 CreateAsset 호출
	// Asset Record 가져와서, AssetRecord 의 CreateAsset 함수 호출
	// (여기서부터는 나의 의지..?) 그리고 실제 prototype load 도 수행하기.
	Asset* asset = nullptr;

	// 원래대로라면 Asset Info 도 따로 만들고, 최초로 만들어준 file id 및 guid 도
	// 해당 info 파일에 넣어줘야 한다.
	// 하지만 현재는 일단 빠르게 개발하기 위해 해당 단계는 건너뛴다.

	if (asset == nullptr)
	{
		FileId fileId(relativePath);

		// 기존 info 지우고
		// info 다시쓰고 ?

		asset = assetProcessor->CreateAsset(fileId, relativePath);
		assetProcessor->onCreate();
	}

	const std::string resAbaPath = GetAbsoluteResourcePath(relativePath);

	//if (needImport)
	//{
	//	const LvString resAbsPath = GetAbsoluteResourcePath(mainAsset->uuid.GetGuid());
	//	const LvString cacheAbsPath = GetAbsoluteCachePath(mainAsset);

	//	record->Import(*mainAsset, resAbsPath, cacheAbsPath);
	//	record->UpdateInfo(mainAsset);

	//	if (mainAsset->IsLoaded())
	//	{
	//		lv_atomic_set(&LvAssetDatabase::_pendingAssetLock, 1);

	//		// 로드가 된 상태에서 파일이 변경된 경우에 header 갱신, SubAsset갱신, Prototype갱신이 이루어져야한다.(서브에셋의 prototype도 포함)
	//		_PendingReloadAssets.Enqueue(mainAsset);

	//		lv_atomic_set(&LvAssetDatabase::_pendingAssetLock, 0);
	//	}
	//	else
	//	{
	//		mainAsset->preload();
	//	}
	//}
	//else
	//{
	//	if (mainAsset->HasCache() && !mainAsset->IsLoaded())
	//	{
	//		mainAsset->preload();
	//	}
	//}
}

void AssetManager::ImportAsset(const std::string& relativePath)
{
}
