#pragma once

#include "Common.h"

// File IDs: Internal identifier for faster lookups within a project (like call numbers).
// File��θ��� ������ Guid �� ���������.
class FileId
{
	friend class IdentifierManager;
public:
	// �Ʒ� 2���� Guid �� local ID ���� �������� ���� �ڵ��̴�.
	// static constexpr size_t FileIdByteCount = 8;
	// static constexpr size_t FileIdStringCount = 16;
	// explicit Guid(const LvString& relationPath, uint32 localID);

	static constexpr size_t FileIdStringCount = 8;
	static constexpr size_t FileIdByteCount = 4;

	struct Hash96
	{
		uint32 hash1;
		uint64 hash2;
	};

	FileId() = default;
	// ���� �⺻ �����ڷ� ������ �ȵǴµ�
	FileId(const FileId& FileId);
	FileId(const std::string& relativePath);
	~FileId() = default;

	inline std::string ToString() const
	{
		char base16[FileIdStringCount + 1];
		toBase16(_bytes, base16);
		return std::string(base16, FileIdStringCount);
	}
	inline uint32 GetHash() const
	{
		uint32 hash1 = 0;
		memcpy(&hash1, &_bytes[0], sizeof(unsigned char) * FileIdByteCount);
		return hash1;
	}
	inline uint32 GetHash32() const
	{
		uint32 hash1 = 0;
		memcpy(&hash1, &_bytes[0], sizeof(unsigned char) * 4);
		return hash1;
	}
	// inline uint64 GetHash64() const
	// {
	// 	uint64 hash2 = 0;
	// 	memcpy(&hash2, &_bytes[4], sizeof(unsigned char) * 8);
	// 	return hash2;
	// }
	// inline Hash96 GetHash96() const
	// {
	// 	Hash96 r;
	// 	r.hash1 = GetHash32();
	// 	r.hash2 = GetHash64();
	// 
	// 	return r;
	// }
private:


	// binary check sum �� base16 ���� ��ȯ�Ѵ�.
	// ���� : base16 �̶� 16���� ���·� ǥ���Ѵٴ� ���� �ǹ��Ѵ�.
	inline static void toBase16(const unsigned char checksum[FileIdByteCount], char out[FileIdStringCount + 1])
	{
		/*
		* % 02 ��, unsigned char �� 2���� 16������ ǥ���ϵ��� �Ѵ�.
		* 
		* 2  : �ּ� 2�ڸ��� ����ϵ��� �Ѵ�.
		* 0  : ���ڸ��� 0 ���� ä���. space �� ä��� ���� �ƴϴ�.
		* 02 : �ּ� 2�ڸ��� ����ϰ�, ���ڸ��� ���� 0 ���� ä���.
		* 
		* ex) unsigned char value = 15;
		*	  printf("%02x\n", value);  // Prints: 0f
		*	 value = 255;
		*	 printf("%02x\n", value);  // Prints: ff
		*/ 

		const auto error = snprintf(out, FileIdStringCount + 1
			// , "%02x%02x%02x%02x%02x%02x%02x%02x"
			, "%02x%02x%02x%02x"
			// , checksum[0], checksum[1], checksum[2], checksum[3], checksum[4], checksum[5], checksum[6], checksum[7]);
			, checksum[0], checksum[1], checksum[2], checksum[3]);

		// snprintf �� �����ϸ� -1 �� �����Ѵ�.
		// assert(-1 != error, "buffer or format is nullptr!!");
		assert(-1 != error);
	}

	unsigned char _bytes[FileIdByteCount] = { 0, };
};


class IdentifierManager
{
	friend class AssetManager;
public :
	static FileId GetFileId(uint32 key);
	static bool CheckContain(uint32 key);
	static void Clear();
private :	
	//key �� guid �� �ؾ� �Ѵ�.
	static void createFileId(const std::string& relativePath);
	static std::unordered_map<uint32, FileId> _fileIdMap;
};
