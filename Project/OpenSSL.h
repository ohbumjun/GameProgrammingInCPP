#pragma once

// https://www.youtube.com/watch?v=PUdJMTu77rY
/*
>> HTTP, FTP, Telnet �� TCP ���������� �̿��Ͽ� �����͸� �ְ���� ��
plain text ���·� ��Ʈ��ũ �󿡼� ���޵ȴ�.

application ~ application ���̿� �����͸� �ְ���� ��
application �� ����ϴ� �ڵ�, ������ �״�� ��Ʈȭ�Ǿ 
������ �ȴٴ� �ǹ��̴�.

������ �߰��� �� �����͵��� �������Ͽ�
�� ������ Ȯ���ϰų� ����ç �� �ִ�.

Ư�� ���ͳ� ���� ��Ʈ��ũ ���� shared ���̶�� ������ �ȴ�.
�̷��� ���� ������ �ִ�.

>> SSL ?
- HTTP �� SSL �̶�� ���� ä���� �߰��� ���� ��.
�̸� ���� �����͸� encryption �� ��.
��, �߰��� �����͸� �ѹ� ���� ��. ��ȣȭ �� ��. �̶�� �����ϸ� �ȴ�.
*/

/*
https://blog.naver.com/sshuikr/221592311475
>> OPENSSL

�߿��� Library 2����
1) LIBSSL
- ssl, tls ����� ���� ����
2) LIBCRYPTO
- ��ȣȭ �˰��� ���� ���� ����
*/

#include <string>
#include <vector>
#include <functional>

namespace httplib { class Result; }

namespace Security
{
	namespace Cryptography
	{
		// sha1�� ���� ũ��� 20�Դϴ�.
		void sha1(const unsigned char* value, size_t length, unsigned char* out);
		std::vector<unsigned char> sha1(const unsigned char* value, size_t lenth);
		inline std::vector<unsigned char> sha1(const std::vector<unsigned char>& value) { return sha1(value.data(), value.size()); }

		std::vector<unsigned char> encrypt_aes(const unsigned char* key, const unsigned char* plaintext, size_t length);
		std::vector<unsigned char> decrypt_aes(const unsigned char* key, const unsigned char* cipher, size_t lenth);
		inline std::vector<unsigned char> decrypt_aes(const unsigned char* key, const std::vector<unsigned char>& cipher) { return decrypt_aes(key, cipher.data(), cipher.size()); }
	}

	namespace Encoding
	{
		std::string base64_encode(const unsigned char* value, size_t length);
		inline std::string base64_encode(const std::vector<unsigned char>& value) { return base64_encode(value.data(), value.size()); }

		size_t base64_decode(const char* input, size_t input_len, std::vector<unsigned char>& output);
		std::vector<unsigned char> base64_decode(const char* input, size_t input_len);
		inline size_t base64_decode(const std::string& value, std::vector<unsigned char>& output) { return base64_decode(value.c_str(), value.length(), output); }
		inline std::vector<unsigned char> base64_decode(const std::string& value) { return base64_decode(value.c_str(), value.length()); }
	}
}
namespace Network
{
	void put_http(const std::string& host, const std::string& path, const std::string& content, const std::string& contentType, httplib::Result& result);
	void get_http(const std::string& host, const std::string& path, httplib::Result& result);
	void get_http_content(const std::string& host, const std::string& path, httplib::Result& result, const std::function<bool(const char* data, size_t data_length)>& content_receiver);
}



