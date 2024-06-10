#include "ImageAssetProcessor.h"
#include "Filesystem.h"
#include "ImageAsset.h"
#include "Image.h"
#include "Renderer.h"
#include "SDL/SDL_image.h"

Asset* ImageAssetProcessor::CreateAsset(const FileId& fileId, const std::string& resourcePath, bool isBuiltIn)
{
	const std::string& name = FileSystem::GetPathName(resourcePath.c_str());

	ImageAsset* asset = new ImageAsset(fileId, resourcePath);

	// writeVersion(asset);

	return asset;
}

void ImageAssetProcessor::DestroyAsset(Asset* asset)
{
}

void ImageAssetProcessor::onLoad(Asset* asset, const std::string& resAbsolutePath)
{
	SDL_Texture* tex = nullptr;

	const std::string& relativePath = asset->GetResourcePath();

	// Load from file
	// Image 를 load 하여 SDL_Surface 안에 담는다.
	SDL_Surface* surf = IMG_Load(relativePath.c_str());
	if (!surf)
	{
		SDL_Log("Failed to load texture file %s", relativePath.c_str());
		return ;
	}

	// Create texture from surface (SDL_Surface 를 SDL_Texture 로 변환)
	// SDL_Texture 가 drawing 을 위해 필요한 것이다.

	SDL_Renderer* renderer = Renderer::GetRenderer();

	tex = SDL_CreateTextureFromSurface(renderer, surf);

	SDL_FreeSurface(surf);

	if (!tex)
	{
		SDL_Log("Failed to convert surface to texture for %s", relativePath.c_str());
		return;
	}

	Image* newImage = new Image(tex);

	asset->SetPrototype(newImage);

	/*
	LvImageAssetInfo* imageInfo = static_cast<LvImageAssetInfo*>(asset.info);

	LV_CHECK(imageInfo->GetPlatformSettings().Count() > 0, "platform setting must have default setting");

	auto& platformSetting = imageInfo->GetActivePlatformSetting(LvPlatformType::STANDALONE);

	if (asset.IsBuiltIn())
	{
		imageInfo->SetImageType(LvImageAssetInfo::ImageType::SPRITE);
		imageInfo->SetTextureFormat(Render::LvTextureFormat::RGBA32);
		platformSetting.compression = LvImageAssetInfo::CompressionQuality::NONE;
	}

	unsigned char sourceAssetChecksum[16];

	LvMemoryStream<> resourceStream;
	LvFileStream fs(resAbsolutePath.c_str(), LvFileMode::OPEN, LvFileAccess::READ_ONLY);
	resourceStream.Resize(fs.Length());
	if (fs.Length() > 0)
	{
		fs.ReadRaw(resourceStream.GetMemory(), fs.Length());
	}
	fs.Close();

	lv_md4(static_cast<const unsigned char*>(resourceStream.GetMemory()), resourceStream.Length(), sourceAssetChecksum);

	unsigned char md5sum[16];
	generateSettingHash(sourceAssetChecksum, LvPlatformType::STANDALONE, imageInfo, &platformSetting, md5sum);

	char md5sumBase16[33];
	lv_md4_checksum_to_base16(md5sum, md5sumBase16);

	writePlatformCompressPrototype(resAbsolutePath, md5sumBase16, archive, asset, LvPlatformType::STANDALONE, &dependencies);

	*/
}
