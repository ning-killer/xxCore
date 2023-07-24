#include "soapUtil.h"
#include <memory.h>
#include <string.h>
#include <time.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>

const char soap_base64o[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char soap_base64i[81] = "\76XXX\77\64\65\66\67\70\71\72\73\74\75XXXXXXX\00\01\02\03\04\05\06\07\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31XXXXXX\32\33\34\35\36\37\40\41\42\43\44\45\46\47\50\51\52\53\54\55\56\57\60\61\62\63";

#define soap_coblank(c)         ((c)+1 > 0 && (c) <= 32)

const char* SoapUtil::soap_base642s(const char* s, char* t, size_t l, int* n)
{
    size_t i, j;
    int c;
    unsigned long m;
    const char* p;
    if (!s || !*s)
    {
        if (n)
            *n = 0;
        return NULL;
    }
    if (!t)
    {
        l = (strlen(s) + 3) / 4 * 3 + 1;    /* space for raw binary and \0 */
        t = new char[l];
    }
    if (!t)
        return NULL;
    p = t;
    if (n)
        *n = 0;
    for (i = 0; ; i += 3, l -= 3)
    {
        m = 0;
        j = 0;
        while (j < 4)
        {
            c = *s++;
            if (c == '=' || !c)
            {
                if (l >= j - 1)
                {
                    switch (j)
                    {
                    case 2:
                        *t++ = (char)((m >> 4) & 0xFF);
                        i++;
                        l--;
                        break;
                    case 3:
                        *t++ = (char)((m >> 10) & 0xFF);
                        *t++ = (char)((m >> 2) & 0xFF);
                        i += 2;
                        l -= 2;
                    }
                }
                if (n)
                    *n = (int)i;
                if (l)
                    *t = '\0';
                return p;
            }
            c -= '+';
            if (c >= 0 && c <= 79)
            {
                int b = soap_base64i[c];
                if (b >= 64)
                    return NULL;

                m = (m << 6) + b;
                j++;
            }
            else if (!soap_coblank(c + '+'))
                return NULL;

        }
        if (l < 3)
        {
            if (n)
                *n = (int)i;
            if (l)
                *t = '\0';
            return p;
        }
        *t++ = (char)((m >> 16) & 0xFF);
        *t++ = (char)((m >> 8) & 0xFF);
        *t++ = (char)(m & 0xFF);
    }
}

std::string SoapUtil::soap_rand_uuid(char* msg)
{
    char buf[1024];
    int r1, r2, r3, r4;
    size_t i;
    static int k = 0xFACEB00C;
    int lo = k % 127773;
    int hi = k / 127773;
    r1 = time(NULL);

    k = 16807 * lo - 2836 * hi;
    if (k <= 0)
        k += 0x7FFFFFFF;
    r2 = k;
    /* k &= 0x8FFFFFFF; */
    auto msgLen = strlen(msg);
    for (i = 0; i < (msgLen < 16UL ? msgLen : 16UL); i++)
        r2 += msg[i];

    RAND_pseudo_bytes((unsigned char*)&r3, sizeof(r3));
    RAND_pseudo_bytes((unsigned char*)&r4, sizeof(r4));
    //r3 = rand();
    //r4 = rand();
    snprintf(buf, 512, "%s%8.8x-%4.4hx-4%3.3hx-%4.4hx-%4.4hx%8.8x", "urn:uuid:", r1, (short)(r2 >> 16),
        (short)(((short)r2 >> 4) & 0x0FFF),
        (short)(((short)(r3 >> 16) & 0x3FFF) | 0x8000),
        (short)r3, r4);
    return std::string(buf);
}

const char* SoapUtil::soap_hex2s(const char* s, char* t, size_t l, int* n)
{
    const char* p;
    if (!s || !*s)
    {
        if (n)
            *n = 0;
        return NULL;
    }
    if (!t)
    {
        l = strlen(s) / 2 + 1;      /* make sure enough space for \0 */
        t = new char[l];
    }
    if (!t)
        return NULL;
    p = t;
    while (l)
    {
        int d1, d2;
        d1 = *s++;
        if (!d1)
            break;
        d2 = *s++;
        if (!d2)
            break;
        *t++ = (char)(((d1 >= 'A' ? (d1 & 0x7) + 9 : d1 - '0') << 4) + (d2 >= 'A' ? (d2 & 0x7) + 9 : d2 - '0'));
        l--;
    }
    if (n)
        *n = (int)(t - p);
    if (l)
        *t = '\0';
    return p;
}

std::string SoapUtil::soap_createTime(time_t n)
{
    tm T;
    tm* pT = &T;
    char tmpBuf[32];
#ifdef _WIN32
    gmtime_s(&T, &n);
    //localtime_s(&T, &n);
#else
    pT = gmtime(&n);
    //pT = localtime(&n);
#endif
    
    strftime(tmpBuf, 32, "%Y-%m-%dT%H:%M:%SZ", pT);
    return std::string(tmpBuf);
}

std::string SoapUtil::soap_strRand()
{
    unsigned int r;
    RAND_pseudo_bytes((unsigned char*)&r, sizeof(unsigned int));
    char a[32];
    snprintf(a, 32, "%u", r);
    return std::string(a);
}

std::string SoapUtil::soap_md5(const std::string& ha)
{
    void* ctx = nullptr;
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    ctx = new EVP_MD_CTX();
    EVP_MD_CTX_init((EVP_MD_CTX*)ctx);
#else
    ctx = (void*)EVP_MD_CTX_new();
#endif
    EVP_DigestInit((EVP_MD_CTX*)ctx, EVP_md5());

    unsigned char hashBin[16];
    EVP_DigestUpdate((EVP_MD_CTX*)ctx, (const void*)ha.c_str(), ha.size());
    EVP_DigestFinal_ex((EVP_MD_CTX*)ctx, hashBin, NULL);

#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    EVP_MD_CTX_cleanup((EVP_MD_CTX*)ctx);
    delete (EVP_MD_CTX*)ctx;
#else
    EVP_MD_CTX_free((EVP_MD_CTX*)ctx);
#endif
    unsigned short i;
    unsigned char j;
    char hashHex[32];
    for (i = 0; i < 16; i++)
    {
        j = (hashBin[i] >> 4) & 0xf;
        if (j <= 9)
            hashHex[i * 2] = (j + '0');
        else
            hashHex[i * 2] = (j + 'a' - 10);
        j = hashBin[i] & 0xf;
        if (j <= 9)
            hashHex[i * 2 + 1] = (j + '0');
        else
            hashHex[i * 2 + 1] = (j + 'a' - 10);
    }

    return std::string(hashHex, 32);
}

std::string SoapUtil::soap_Nonce()
{
    time_t r = time(NULL);
    char strTime[32];
    int len = snprintf(strTime, 32, "%ld", r);
    return soap_md5(std::string(strTime, len));
}