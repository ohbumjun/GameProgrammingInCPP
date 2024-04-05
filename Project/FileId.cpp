#include "FileId.h"
#include "SpookyHash.h"

FileId::FileId(const std::string& relationPath)
{
	const uint32 fileHash = SpookyHash::Hash32(relationPath.c_str(), relationPath.length(), 0);
	memcpy(&_bytes[0], &fileHash, sizeof(uint32));
}

FileId IdentifierManager::GetFileId(uint32 key)
{
	assert(CheckContain(key));
	return _fileIdMap[key];
}

bool IdentifierManager::CheckContain(uint32 key)
{
	return _fileIdMap.find(key) != _fileIdMap.end();
}

void IdentifierManager::Clear()
{
	_fileIdMap.clear();
}

void IdentifierManager::createFileId(const std::string& relativePath)
{
}