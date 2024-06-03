#include "Gat1400Util.hpp"
#include <mbedtls/md5.h>
#include <mbedtls/aes.h>
#include <chrono>
#include <ctime>

using namespace Emx;

// 取得[a,b]之间的随机整数，使用（rand() % (b-a+1)）+ a (结果值将含a含b)
#define random_2(a,b) ((rand()%(b-a+1))+a)

std::string Gat1400Util::GetNonceStr(const int size) {
    std::string retStr = "";
    static const char nonceChar[37] = "abcdefghijklmnopqrstuvwxyz0123456789";
    srand((int)time(0));
    for (int i = 0; i < size; i++) {
        char tmp[2] = { 0 };
        int index = random_2(0, 35);
        sprintf(tmp, "%c", nonceChar[index]);
        retStr.append(tmp);
    }
    return retStr;
}

bool Gat1400Util::GetMd5(const char* src, std::string &md5) {
    char output[16] = { 0 };
    char buf[33] = { 0 };
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, (const unsigned char *)src, strlen(src));
    mbedtls_md5_finish(&ctx, (unsigned char *)output);
    mbedtls_md5_free(&ctx);
    for (int i = 0; i < 16; i++) {
        snprintf(buf + i * 2, sizeof(buf) - i * 2, "%02x", output[i]);
    }
    md5 = std::string(buf);
    return true;
}

std::string Gat1400Util::GetMapValueByKey(const std::string &key, std::map<std::string, std::string> &srcMap) {
    for (auto object : srcMap) {
        if (object.first == key) {
            return object.second;
        }
    }
    return "";
}

bool Gat1400Util::GetSign(const std::string nonce, const std::string devId, const std::string pwd, std::string &sign) {
    /**
        签名信息，使用AES对nonce进行加密
        签名步骤：
        1.  aesKey = md5({deviceId}:aiplatform:{password}),{}代表变量，{}中的字段替换为实际值，不包含md5计算不包含{}
        2.  aes = AES(aesKey)
        3.  sign = hex(aes.encrypt(nonce))
    */
    // 获取aeskey
    std::string aesKey = "";
    char tmpBuf[512] = { 0 };
    snprintf(tmpBuf, sizeof(tmpBuf) - 1, "%s:aiplatform:%s", devId.c_str(), pwd.c_str());
    if (!GetMd5(tmpBuf, aesKey)) {
        return false;
    }
    // 加密
    std::string out;
    std::string nonceTmp = nonce;
    Pkcs5Padding(nonceTmp);
    if (!AesEcbPcsk5Encrypt(nonceTmp, out, (char *)aesKey.c_str(), aesKey.size())) {
        return false;
    }
    //转换为16进制字符串
    char signBuf[out.size() * 2 + 1] = { 0 };
    byteToHex((const unsigned char *)out.c_str(), out.size(), signBuf);
    signBuf[out.size() * 2] = '\0';
    for (int i = 0; i < int(out.size() * 2); i++) {
        signBuf[i] = tolower(signBuf[i]);
    }
    sign = std::string(signBuf);
    return true;
}

void Gat1400Util::Pkcs5Padding(std::string &data) {
    size_t data_len = data.size();
    size_t pad_len = 16 - (data_len % 16); 
    std::string padded_data = data + std::string(pad_len, static_cast<char>(pad_len));
    data = padded_data;
}

bool Gat1400Util::AesEcbPcsk5Encrypt(const std::string &in, std::string &out, char *key, int key_len) {
    bool ret = false;
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    do {
        // mbedtls实现AES ECB PKCS5 Padding加密
        // mbedtls库目前只支持AES-128(16位密钥)和AES-256加密算法(32位密钥)
        int keylenSet = key_len * 8;
        if (keylenSet != 128 && keylenSet != 256) {
            emxloge("mbedtls only support AES-128 & AES-256, unsupport AES-%d.\n", keylenSet);
            break;
        }
        // 加密
        if (mbedtls_aes_setkey_enc(&ctx, (const unsigned char*)key, keylenSet) != 0) {
            emxloge("mbedtls_aes_setkey_enc failed!\n");
            break;
        }
        
        //note: mbedtls_aes_crypt_ecb奇葩的接口设计：1. input & output >= 16字节；2. input大小 <= output大小
        //note: 为防止外部传入input&output无法校验控制大小，所以接管对buffer的控制
        int SrcSize = in.size();
        if (SrcSize == 0) {
            emxloge("Input is empty.\n");
            break;
        }
        std::string output;
        output.resize(SrcSize);
        std::string input = in;
        for (int i = 0; i < SrcSize; i += 16) {
            mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT,
                              reinterpret_cast<const unsigned char *>(input.data() + i),
                              (unsigned char *)(output.data() + i));
        }
        out = output;
        ret = true;
    } while (false);
    mbedtls_aes_free(&ctx);
    return ret;
}

void Gat1400Util::byteToHex(const unsigned char *input, int length, char *output) {
    const char *hex = "0123456789abcdef";
    for (int i = 0; i < length; i++) {
        output[i * 2] = hex[(input[i] >> 4) & 0x0F];
        output[i * 2 + 1] = hex[input[i] & 0x0F];
    }
}

// yyyyMMddHHmmss
std::string Gat1400Util::GetTimeStr(TimePrecision precision) {
    unsigned long long tmp = 0;
    tm* local;
    char buf[15] = { 0 };  
    if (precision == MS) {
        tmp = Time::GetS();
    } else if (precision == S) {
        tmp = Time::GetS();
    }
    std::time_t t(tmp);
    local = localtime(&t);
    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", local);
    return std::string(buf);
}

char *Gat1400Util::GetTimeStr(uint32_t utc, char *timeStr, int maxSize) {
    auto timep = (time_t) utc;
    struct tm *p = localtime(&timep);
    snprintf(timeStr, maxSize, "%04d-%02d-%02d_%02d-%02d-%02d",
             (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return timeStr;
}

int Gat1400Util::GetRandom(int a, int b) {
    static int init = 0;
    if (init == 0) {
        srand((int)time(NULL));
        init = 1;
    }
    return random_2(a, b);
}

std::string Gat1400Util::GetImgType(ImgType type) {
    std::string s_type = "jpg";
    switch (type)
    {
    case JPG:
        s_type = "jpg";
        break;
    case BMP:
        s_type = "bmp";
        break;
    case PNG:
        s_type = "png";
        break;
    default:
        break;
    }
    return s_type;
}

int Gat1400Util::m_index = 0;