/*
 * @Author: xiong
 * @Date: 2023/2/4
*/
#ifdef EMX_SSL_MBEDTLS

#include "EmxCrypto.hpp"

#include "mbedtls/version.h"

#if !defined(MBEDTLS_CONFIG_FILE)

#include "mbedtls/config.h"

#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)

#include "mbedtls/platform.h"

#else

#include <stdio.h>

#define mbedtls_printf     printf
#endif

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_PK_PARSE_C) && \
    defined(MBEDTLS_FS_IO) && defined(MBEDTLS_ENTROPY_C) && \
    defined(MBEDTLS_CTR_DRBG_C)

#include "mbedtls/error.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#endif

#if MBEDTLS_VERSION_NUMBER >= 0x02100900
#endif

using namespace Emx;

ErrCodeE EmxCrypto::RsaPubKeyDecrypt(const char *pubKeyPath, const char *src, int srcSize, std::string &dst) {
    ErrCodeE ret = ErrCodeE::Failure;
    do {
        if (pubKeyPath == nullptr || src == nullptr || srcSize < 1) {
            emxloge("input data is error\n");
            break;
        }

        //note: 读取公钥
        std::string pubKey;
        if (File::Read(pubKeyPath, pubKey) != ErrCodeE::Success) {
            emxloge("read pubKey file failed\n");
            break;
        }

        //note: 解析公钥至pk
        mbedtls_pk_context pk;
        mbedtls_pk_init(&pk);
        if (0 != mbedtls_pk_parse_public_key(&pk, (const unsigned char *)pubKey.data(), pubKey.size() + 1)) {
            emxloge("mbedtls_pk_parse_public_key failed\n");
            mbedtls_pk_free(&pk);
            break;
        }

        //note: pk导入公钥rsa
        mbedtls_rsa_context rsa;
        mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);
        rsa = *(mbedtls_rsa_context*)pk.pk_ctx;
        size_t len = (size_t)srcSize;

        //note: 源数据解密
        int output_maxlen = 512;
        char output[output_maxlen] = { 0 };
        if (0 == mbedtls_rsa_rsaes_pkcs1_v15_decrypt(&rsa, NULL, NULL
                        , MBEDTLS_RSA_PUBLIC, (size_t*)&len
                        , (const unsigned char *)src
                        , ( unsigned char*)output, output_maxlen)) {
            dst.append(std::string(output, output_maxlen));
            ret = ErrCodeE::Success;
            emxlogd("RsaPubKeyDecrypt ok\n");
        } else{
            emxloge("mbedtls_rsa_rsaes_pkcs1_v15_decrypt failed\n");
        }
        mbedtls_rsa_free(&rsa);
    } while(false);
    return ret;
}

ErrCodeE EmxCrypto::RsaPubKeyDecrypt(const std::string &pubKeyPath, const std::string &src,  unsigned char *dst, const int dst_maxlen) {
    ErrCodeE ret = ErrCodeE::Failure;
    do {
        if (pubKeyPath.size() < 1 || src.size() < 1) {
            emxloge("input data is error, pubkeypath(%s), src size(%s)\n"
                                        , pubKeyPath.c_str(), src.size());
            break;
        }

        //note: 读取公钥
        std::string pubKey;
        if (File::Read(pubKeyPath.c_str(), pubKey) != ErrCodeE::Success) {
            emxloge("read pubKey file failed\n");
            break;
        }

        //note: 解析公钥至pk
        mbedtls_pk_context pk;
        mbedtls_pk_init(&pk);
        if (0 != mbedtls_pk_parse_public_key(&pk, (const unsigned char *)pubKey.data(), pubKey.size() + 1)) {
            emxloge("mbedtls_pk_parse_public_key failed\n");
            mbedtls_pk_free(&pk);
            break;
        }

        //note: pk导入公钥rsa
        mbedtls_rsa_context rsa;
        mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);
        rsa = *(mbedtls_rsa_context*)pk.pk_ctx;
        size_t len = (size_t)src.size();

        //note: 源数据解密
        if (0 == mbedtls_rsa_rsaes_pkcs1_v15_decrypt(&rsa, NULL, NULL
                        , MBEDTLS_RSA_PUBLIC, (size_t*)&len
                        , (const unsigned char *)src.data()
                        , ( unsigned char*)dst, dst_maxlen)) {
            ret = ErrCodeE::Success;
            emxlogd("RsaPubKeyDecrypt ok\n");
        } else{
            emxloge("mbedtls_rsa_rsaes_pkcs1_v15_decrypt failed\n");
        }
        mbedtls_rsa_free(&rsa);
    } while(false);
    return ret;
}

#endif
