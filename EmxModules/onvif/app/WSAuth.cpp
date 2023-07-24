#include "WSAuth.h"
#include <openssl/hmac.h>
#include "soapUtil.h"
#ifndef _WIN32
#include <string.h>
#endif

WSAuth::WSAuth()
{
}

WSAuth::~WSAuth()
{
}

WSAuth& WSAuth::Instance()
{
    static std::shared_ptr<WSAuth> g_instance(new WSAuth);
    static WSAuth& g_instance_ref = *g_instance;
    return g_instance_ref;
}

void WSAuth::setPassword(const std::string& pwd)
{
	_password = pwd;
}

bool WSAuth::VerifyPassword(const std::string& cipherPassword, const std::string& nonce, const std::string& createdTime)
{
    void* ctx = nullptr;
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    ctx = new EVP_MD_CTX();
    EVP_MD_CTX_init((EVP_MD_CTX*)ctx);
#else
    ctx = (void*)EVP_MD_CTX_new();
#endif
    EVP_DigestInit((EVP_MD_CTX*)ctx, EVP_sha1());

    char HA1[20];
    char HA2[20];

    int noncelen;
    auto plainNonce = SoapUtil::soap_base642s(nonce.c_str(), NULL, 0, &noncelen);

    EVP_DigestUpdate((EVP_MD_CTX*)ctx, (const void*)plainNonce, noncelen);
    EVP_DigestUpdate((EVP_MD_CTX*)ctx, (const void*)createdTime.c_str(), createdTime.size());
    EVP_DigestUpdate((EVP_MD_CTX*)ctx, (const void*)_password.c_str(), _password.size());
    EVP_DigestFinal_ex((EVP_MD_CTX*)ctx, (unsigned char*)HA1, NULL);
    
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    EVP_MD_CTX_cleanup((EVP_MD_CTX*)ctx);
    delete (EVP_MD_CTX*)ctx;
#else
    EVP_MD_CTX_free((EVP_MD_CTX*)ctx);
#endif

    SoapUtil::soap_base642s(cipherPassword.c_str(), HA2, 20, NULL);
    delete[] plainNonce;

    if (memcmp(HA1, HA2, 20))
    {
        return false;
    }

    return true;
}