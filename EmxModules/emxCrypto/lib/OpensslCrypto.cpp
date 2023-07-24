/*
 * @Author: xiong
 * @Date: 2023/2/4
*/
#ifdef EMX_SSL_OPENSSL

#include "EmxCrypto.hpp"
#include "openssl/opensslv.h"
#include "openssl/md5.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"

using namespace Emx;

#if  OPENSSL_VERSION_NUMBER >= 0x1000214fL
#endif

#define BEGIN_RSA_PUBLIC_KEY    "BEGIN RSA PUBLIC KEY"

ErrCodeE EmxCrypto::RsaPubKeyDecrypt(const char *pubKeyPath, const char *src, int srcSize, std::string &dst) {
    ErrCodeE e = ErrCodeE::Failure;
    std::string pubKey;
    if (File::Read(pubKeyPath, pubKey) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    auto pKeyBio = BIO_new_mem_buf(pubKey.c_str(), (int) pubKey.size());
    if (!pKeyBio) {
        emxloge("malloc bio failed\n");
        return ErrCodeE::Failure;
    }

    auto *pRsa = RSA_new();
    if (strstr(pubKey.data(), BEGIN_RSA_PUBLIC_KEY)) {
        pRsa = PEM_read_bio_RSAPublicKey(pKeyBio, &pRsa, nullptr, nullptr);
    } else {
        pRsa = PEM_read_bio_RSA_PUBKEY(pKeyBio, &pRsa, nullptr, nullptr);
    }

    if (!pRsa) {
        emxloge("malloc rsa failed\n");
        goto end;
    }

    dst.resize(RSA_size(pRsa));
    //解密
    if (RSA_public_decrypt(srcSize,
                           (const unsigned char *) (src),
                           (unsigned char *) dst.data(),
                           pRsa,
                           RSA_PKCS1_PADDING) >= 0)
        e = ErrCodeE::Success;
    end:
    BIO_free_all(pKeyBio);
    if (pRsa) RSA_free(pRsa);
    return e;
}

ErrCodeE EmxCrypto::RsaPubKeyDecrypt(const std::string &pubKeyPath, const std::string &src,  unsigned char *dst, const int dst_maxlen) {
    emxloge("openssl no support this api!\n");
    return ErrCodeE::Failure;
}

#endif