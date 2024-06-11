#pragma once

// https://www.youtube.com/watch?v=PUdJMTu77rY
/*
>> HTTP, FTP, Telnet 등 TCP 프로토콜을 이용하여 데이터를 주고받을 때
plain text 형태로 네트워크 상에서 전달된다.

application ~ application 사이에 데이터를 주고받을 때
application 이 사용하는 코드, 데이터 그대로 비트화되어서 
전달이 된다는 의미이다.

문제는 중간에 그 데이터들을 스위핑하여
그 내용을 확인하거나 가로챌 수 있다.

특히 인터넷 같이 네트워크 망이 shared 망이라면 문제가 된다.
이러한 보안 문제가 있다.

>> SSL ?
- HTTP 에 SSL 이라는 보안 채널을 중간에 붙인 것.
이를 통해 데이터를 encryption 한 것.
즉, 중간에 데이터를 한번 감싼 것. 암호화 한 것. 이라고 생각하면 된다.
*/

/*
https://blog.naver.com/sshuikr/221592311475
>> OPENSSL

중요한 Library 2가지
1) LIBSSL
- ssl, tls 기능을 위해 사용됨
2) LIBCRYPTO
- 암호화 알고리즘 등을 위해 사용됨
*/

#include <string>
#include <vector>
#include <functional>

namespace httplib { class Result; }

namespace Security
{
	namespace Cryptography
	{
		// sha1의 버퍼 크기는 20입니다.
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



