#include "OpenSSL.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../External/httplib.h"

// #include <rapidjson/document.h>

#include "Encoding.h"

void Security::Cryptography::sha1(const unsigned char* value, size_t length, unsigned char* out)
{
    SHA1(value, length, out);
}

std::vector<unsigned char> Security::Cryptography::sha1(const unsigned char* value, size_t lenth)
{
    std::vector<unsigned char> obuf(SHA_DIGEST_LENGTH, SHA_DIGEST_LENGTH);

    sha1(value, lenth, obuf.data());

    return obuf;
}

std::vector<unsigned char> Security::Cryptography::encrypt_aes(const unsigned char* key, const unsigned char* plaintext, const size_t length)
{
    std::vector<unsigned char> result;

    unsigned char iv[16] = { 0, }; // 16바이트 (128비트)

    // EVP_CIPHER_CTX 구조체 생성 및 초기화
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        ERR_print_errors_fp(stderr);
        return {};
    }

    // 암호화 알고리즘 설정
    if (EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv, 1) != 1) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_set_padding(ctx, EVP_PADDING_PKCS7);

    // 암호화된 데이터 버퍼 크기 계산
    int ciphertext_len = EVP_CIPHER_CTX_block_size(ctx) * (length + 1);

    result.resize(ciphertext_len);
    unsigned char* ciphertext = result.data();

    // 암호화
    int update_len;
    if (EVP_CipherUpdate(ctx, ciphertext, &update_len, plaintext, length) != 1) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 마지막 암호화 블록 처리
    int final_len;
    if (EVP_CipherFinal_ex(ctx, ciphertext + update_len, &final_len) != 1) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    result.resize(update_len + final_len);

    // 암호화된 데이터 출력
    // printf("Ciphertext: ");
    // for (int i = 0; i < update_len + final_len; i++) {
    //     printf("%02x", ciphertext[i]);
    // }

    // EVP_CIPHER_CTX 구조체 해제
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

std::vector<unsigned char> Security::Cryptography::decrypt_aes(const unsigned char* key, const unsigned char* cipher, size_t length)
{
    std::vector<unsigned char> result;

    // 오류 처리
    ERR_load_crypto_strings();

    // 키 및 초기화 벡터
    unsigned char iv[16] = { 0, }; // 16바이트 (128비트)

    EVP_CIPHER_CTX* ctx;
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
    }

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv))
    {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
    }

    std::vector<unsigned char> s;
    s.resize(length + EVP_CIPHER_CTX_block_size(ctx));
    unsigned char* plaintext = s.data();
    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    int len;
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, cipher, length))
    {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
    }

    int plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
    }
    plaintext_len += len;
    s.resize(plaintext_len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return s;
}


std::string Security::Encoding::base64_encode(const unsigned char* value, size_t length)
{
    const int shaBase64Len = base64_encode_size(length);

    std::string shaBase64("", shaBase64Len);
    ::base64_encode(const_cast<char*>(shaBase64.c_str()), value, length);

    assert(shaBase64[shaBase64.length() - 1] == '\0', "Wrong string");

    return shaBase64;
}

size_t Security::Encoding::base64_decode(const char* input, size_t input_len, std::vector<unsigned char>& output)
{
    // BIO 객체 생성
    BIO* bio = BIO_new(BIO_f_base64());
    if (bio == nullptr) {
        ERR_print_errors_fp(stderr);
        return 0;
    }

    // BIO 객체 설정
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(bio, BIO_new_mem_buf((void*)input, input_len));


    // 출력 버퍼 할당
    output.resize(input_len);

    // 디코딩 수행
    size_t decoded_len = BIO_read(bio, output.data(), input_len);
    if (decoded_len <= 0) {
        ERR_print_errors_fp(stderr);
        BIO_free(bio);
        return 0;
    }
    output.resize(decoded_len);

    // BIO 객체 해제
    BIO_free(bio);

    return decoded_len;
}

std::vector<unsigned char> Security::Encoding::base64_decode(const char* input, size_t input_len)
{
    std::vector<unsigned char> decode;
    base64_decode(input, input_len, decode);
    return decode;
}


void Network::put_http(const std::string& host, const std::string& path, const std::string& content, const std::string& contentType, httplib::Result& result)
{
    httplib::Client cli(host.c_str());

    // TODO 나중에 제거 필요
    // 인증서 확인 비활성화
    cli.enable_server_certificate_verification(false);

    const std::string _path = path.c_str();
    const std::string _content = content.c_str();
    const std::string _contentType = contentType.c_str(); // https://docs.unity3d.com/ScriptReference/Networking.UnityWebRequest.Put.html;

    result = cli.Put(_path, _content, _contentType);
}

void Network::get_http(const std::string& host, const std::string& path, httplib::Result& result)
{
    httplib::Client cli(host.c_str());

    // TODO 나중에 제거 필요
    // 인증서 확인 비활성화
    cli.enable_server_certificate_verification(false);

    const std::string _path = path.c_str();

    result = cli.Get(path.c_str());
}

void Network::get_http_content(const std::string& host, const std::string& path, httplib::Result& result, const std::function<bool(const char* data, size_t data_length)>& content_receiver)
{
    httplib::Client cli(host.c_str());

    // TODO 나중에 제거 필요
    // 인증서 확인 비활성화
    cli.enable_server_certificate_verification(false);

    const std::string _path = path.c_str();

    result = cli.Get(path.c_str(), content_receiver);
}