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
	// �̸� �ʿ��� ���ϵ��� �ϴ� �ε��Ѵ�.

	// �׸��� Cache ���Ϸ� �����Ѵ�.

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

	// �� type ���� Asset Record ��ü�� �������ش�.
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
	// extension �� ���� asset type ����
	AssetType assetType = AssetExtension::GetAssetTypeByExtension(relativePath);

	// "relativePath" �� asset ��������


	// �ش� asset type ���� asset record ��ü ��������
	AssetProcessor* assetProcessor = _loaders[assetType];

	// ������ ���� Load
	// �̶� guid �� uuid �� �߱����ֱ�
	// �ش� ������ CreateAsset ȣ��
	// Asset Record �����ͼ�, AssetRecord �� CreateAsset �Լ� ȣ��
	// (���⼭���ʹ� ���� ����..?) �׸��� ���� prototype load �� �����ϱ�.
	Asset* asset = nullptr;

	// ������ζ�� Asset Info �� ���� �����, ���ʷ� ������� file id �� guid ��
	// �ش� info ���Ͽ� �־���� �Ѵ�.
	// ������ ����� �ϴ� ������ �����ϱ� ���� �ش� �ܰ�� �ǳʶڴ�.

	if (asset == nullptr)
	{
		FileId fileId(relativePath);

		// ���� info �����
		// info �ٽþ��� ?

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

	//		// �ε尡 �� ���¿��� ������ ����� ��쿡 header ����, SubAsset����, Prototype������ �̷�������Ѵ�.(���꿡���� prototype�� ����)
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
