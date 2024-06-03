//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Crypto.hpp"
#include "core/EmxLog.hpp"

using namespace Emx;


void AES_CBC::Init(const uint8_t *key, const uint8_t *iv) {
    AES_init_ctx_iv(&m_ctx, key, iv);
}

void AES_CBC::Encrypt(uint8_t *buf, size_t length) {
    AES_CBC_encrypt_buffer(&m_ctx, buf, length);
}

void AES_CBC::Decrypt(uint8_t *buf, size_t length) {
    if (length % 16 != 0) {
        printf("no 16-byte alignment\n");
        return;
    } 
    AES_CBC_decrypt_buffer(&m_ctx, buf, length);
}

TEA::TEA() {
    int i;
    uint32_t result;
    uint8_t t[] = {0xBC, 0x15, 0x5B, 0x9B};
    uint32_t table_key = 0xdeadbeef;

    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for (i = 0; i < 4; i++) {
        t[i] ^= k1;
        t[i] ^= k2;
        t[i] ^= k3;
        t[i] ^= k4;
    }

    result = t[3];
    result |= t[2] << 8;
    result |= t[1] << 16;
    result |= t[0] << 24;
    m_delta = result;
}

#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (teaKey[(p & 3) ^ e] ^ z)))

void TEA::Encrypt(const uint8_t key[16], const uint8_t iv[16], uint32_t tea[32]) const {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    static uint32_t teaKey[4] = {0x45, 0x41, 0x50, 0x494C};//代表 EAPIL 0x494c="IL"
    int n = 32;
    uint32_t *v = tea;

    for (int i = 0; i < 16; i++) {
        v[i] = key[i];
        v[i + 16] = iv[i];
    }

    rounds = 6 + 52 / n;
    sum = 0;
    z = v[n - 1];
    do {
        sum += m_delta;
        e = (sum >> 2) & 3;
        for (p = 0; p < (unsigned) n - 1; p++) {
            y = v[p + 1];
            z = v[p] += MX;
        }
        y = v[0];
        z = v[n - 1] += MX;
    } while (--rounds);
}

void TEA::Decrypt(const uint32_t tea[32], uint8_t key[16], uint8_t iv[16]) const {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    static uint32_t teaKey[4] = {0x45, 0x41, 0x50, 0x494C};//代表 EAPIL 0x494c="IL"
    int n = 32;

    uint32_t v[32] = {};
    for (int i = 0; i < 32; i++)
        v[i] = tea[i];

    rounds = 6 + 52 / n;
    sum = rounds * m_delta;
    y = v[0];
    do {
        e = (sum >> 2) & 3;
        for (p = n - 1; p > 0; p--) {
            z = v[p - 1];
            y = v[p] -= MX;
        }
        z = v[n - 1];
        y = v[0] -= MX;
        sum -= m_delta;
    } while (--rounds);

    for (int i = 0; i < 16; i++) {
        key[i] = v[i];
        iv[i] = v[i + 16];
    }
}

void Pkcs7Padding::Pad(std::string &data) {
    auto size = data.size();
    int padNum = 16 - ((int) size % 16);
    data.append(padNum, (char) padNum);
}

int Pkcs7Padding::Pad(uint8_t *in, int inSize, uint8_t *out, int &outSize) {
    int padNum = 16 - ((int) inSize % 16);
    if (inSize + padNum > outSize) {
        emxloge("inSize[%d] + padNum[%d] > outSize[%d]\n", inSize, padNum, outSize);
        return -1;
    }
    memcpy(out, in, inSize);
    for (int i = 0; i < padNum; i++)
        out[inSize + i] = padNum;
    return outSize = inSize + padNum;
}

void Pkcs7Padding::UnPad(std::string &data) {
    if ((int)data.size() % 16 != 0) {
        printf("no 16-byte alignment\n");
        return;
    } 
    auto pad = data[data.size() - 1];
    if (pad > (int) data.size()) {
        emxloge("UnPad error, pad size[%d] > data size[%d]\n", pad, (int) data.size());
        return;
    }
    data.resize(data.size() - pad);
}

int Pkcs7Padding::UnPad(uint8_t *data, int &size) {
    if (size % 16 != 0) {
        printf("no 16-byte alignment\n");
        return -1;
    }
    auto pad = data[size - 1];
    if (pad > (int) size) {
        emxloge("UnPad error, pad size[%d] > data size[%d]\n", pad, size);
        return -1;
    }
    return size = size - pad;
}