#include "AssetProcessor.h"

void AssetProcessor::Import(Asset& asset, const std::string& resAbsolutePath, const std::string& cacheAbsolutePath)
{
	// LvHashtable<uint64, LvAssetReference> assetDependencies;
	// LvMemoryStream<> contentStream;
	// {
	// 	LvOutputStream contentOutputStream(&contentStream);
	// 	Engine::LvBinaryArchive contentArchive(contentOutputStream);
	// 	const bool isSucceed = onImport(asset, resAbsolutePath, contentArchive, assetDependencies);
	// 	if (isSucceed == false) return;
	// }
	// 
	// updateHeader(asset, resAbsolutePath);
	// 
	// for (const auto& each : assetDependencies)
	// {
	// 	Editor::LvAsset::Header::Dependency def;
	// 	def.id = each.value.id;
	// 	def.name = each.value.GetName();
	// 	def.type = each.value.GetDependenceType();
	// 	def.refCount = each.value.Count();
	// 	def.uuid = each.value.GetUuid().ToString();
	// 
	// 	asset.header.dependencies.Add(def.id, std::move(def));
	// }
	// 
	// LvMemoryStream<> stream;
	// LvOutputStream os(&stream);
	// 
	// asset.header.Write(os, contentStream.Length());
	// 
	// contentStream.SetPosition(0);
	// os.stream->WriteRaw(contentStream.GetMemory(), contentStream.Length());
	// 
	// LvFileStream fileStream(cacheAbsolutePath.c_str(), LvFileMode::CREATE);
	// fileStream.WriteRaw(stream.GetMemory(), stream.GetPosition());
	// fileStream.Flush();
	// fileStream.Close();
}

bool AssetProcessor::CanRecord(const std::string& resourcePath)
{
    return false;
}

void AssetProcessor::ImportResource(Asset& asset, const std::string& resAbsolutePath)
{
}

Asset* AssetProcessor::CreateAsset(const FileId& fileId, const std::string& relativResourcePath, bool isBuiltIn)
{
    return nullptr;
}
