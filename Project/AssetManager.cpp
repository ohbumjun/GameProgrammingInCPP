#include "AssetManager.h"
#include "AssetExtension.h"
#include "SpriteAssetProcessor.h"


std::unordered_map<AssetType, AssetProcessor*> AssetManager::_loaders;

void AssetManager::initializeProcessors()
{
	_loaders[AssetType::SPRITE] = new SpriteAssetProcessor();
}

void AssetManager::initialize()
{
	AssetExtension::initialize();

	TextureAssetManager::initialize();
}

void AssetManager::finalize()
{
	AssetExtension::finalize();

	TextureAssetManager::finalize();

	// �� type ���� Asset Record ��ü�� �Ҵ����ش�.
}

void AssetManager::createAsset()
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

void AssetManager::LoadAsset(const std::string& relativePath)
{
	// extension �� ���� asset type ����
	AssetType assetType = AssetExtension::GetAssetTypeByExtension(relativePath);

	// �ش� asset type ���� asset record ��ü ��������
	AssetProcessor* assetProcessor = _loaders[assetType];

	// "relativePath" �� asset ��������
	// ������ ���� Load
	// �̶� guid �� uuid �� �߱����ֱ�
	// �ش� ������ CreateAsset ȣ��
	// Asset Record �����ͼ�, AssetRecord �� CreateAsset �Լ� ȣ��
	// (���⼭���ʹ� ���� ����..?) �׸��� ���� prototype load �� �����ϱ�.
}

void AssetManager::ImportAsset(const std::string& relativePath)
{
}
