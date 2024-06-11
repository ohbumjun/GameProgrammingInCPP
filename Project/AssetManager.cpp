#include "AssetManager.h"
#include "AssetExtension.h"
#include "SpriteAssetProcessor.h"
#include "ImageAssetProcessor.h"
#include "EditorContext.h"
#include "FileSystem.h"
#include "FileManager.h"

std::unordered_map<AssetType, AssetProcessor*> AssetManager::_loaders;
std::unordered_map<std::string/*Resources/~~ ��� ����� ?*/, Asset*> AssetManager::_assets;

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

	for (const auto& assetInfo : _assets)
	{
		Asset* asset = assetInfo.second;
		AssetType assetType = asset->GetAssetType();
		AssetProcessor* assetProcessor = _loaders[assetType];
		assetProcessor->DestroyAsset(asset);
	}

	_assets.clear();
}

void AssetManager::CreateAsset()
{
}

// SDL_Texture* AssetManager::LoadTexture(const std::string& relativePath)
// {
// 	return TextureAssetManager::loadTexture(relativePath);
// }
// 
// SDL_Texture* AssetManager::GetTexture(const std::string& fileName)
// {
// 	return TextureAssetManager::getTexture(fileName);
// }

Asset* AssetManager::GetAssetByPath(const std::string& assetPath)
{
	if (_assets.find(assetPath) == _assets.end())
	{
		return nullptr;
	}

	return _assets[assetPath];
}

void AssetManager::DeleteAsset(Asset* asset)
{
	AssetType assetType = asset->GetAssetType();
	AssetProcessor* assetProcessor = _loaders[assetType];
	assetProcessor->DestroyAsset(asset);

	// �� �κп� ����ȭ�� ��������� �Ѵ�.
	_assets.erase(asset->GetResourcePath());
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

	if (_assets.find(relativePath) != _assets.end())
	{
		asset = _assets[relativePath];
	}

	// ������ζ�� Asset Info �� ���� �����, ���ʷ� ������� file id �� guid ��
	// �ش� info ���Ͽ� �־���� �Ѵ�.
	// ������ ����� �ϴ� ������ �����ϱ� ���� �ش� �ܰ�� �ǳʶڴ�.

	if (asset == nullptr)
	{
		FileId fileId(relativePath);

		// ���� info �����
		// info �ٽþ��� ?

		asset = assetProcessor->CreateAsset(fileId, relativePath);
		asset->onCreate();
	}

	const std::string resAbsPath = GetAbsoluteResourcePath(relativePath);
	assetProcessor->onLoad(asset, resAbsPath);

	_assets[relativePath] = asset;

	/*
	LvHashtable<uint64, LvAssetReference> assetDependencies;
	LvMemoryStream<> contentStream;
	{
		LvOutputStream contentOutputStream(&contentStream);
		Engine::LvBinaryTypedArchive contentArchive(contentOutputStream);
		const bool isSucceed = onImport(asset, resAbsolutePath, contentArchive, assetDependencies);
		contentArchive.Flush();
		if (isSucceed == false) return;
	}

	updateHeader(asset, resAbsolutePath);

	for (const auto& each : assetDependencies)
	{
		Editor::LvAsset::Header::Dependency def;
		def.id = each.value.id;
		def.name = each.value.GetName();
		def.type = each.value.GetDependenceType();
		def.refCount = each.value.Count();
		def.uuid = each.value.GetUuid().ToString();

		asset.header.dependencies.Add(def.id, std::move(def));
	}

	LvMemoryStream<> stream;
	LvOutputStream os(&stream);

	asset.header.Write(os, contentStream.Length());

	contentStream.SetPosition(0);
	os.stream->WriteRaw(contentStream.GetMemory(), contentStream.Length());

	LvFileStream fileStream(cacheAbsolutePath.c_str(), LvFileMode::CREATE);
	fileStream.WriteRaw(stream.GetMemory(), stream.GetPosition());
	fileStream.Flush();
	fileStream.Close();
	*/
}

void AssetManager::ImportAsset(const std::string& relativePath)
{
}
