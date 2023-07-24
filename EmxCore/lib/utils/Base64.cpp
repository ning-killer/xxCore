//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Base64.hpp"
#include <cstring>

using namespace Emx;


const char *Base64::trans = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *Base64::Encode(const uint8_t *data, int size, char *base64) {
    int i, j;
    unsigned char current;

    for (i = 0, j = 0; i < size; i += 3) {
        current = (data[i] >> 2);
        current &= (unsigned char) 0x3F;
        base64[j++] = trans[(int) current];

        current = ((unsigned char) (data[i] << 4)) & ((unsigned char) 0x30);
        if (i + 1 >= size) {
            base64[j++] = trans[(int) current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ((unsigned char) (data[i + 1] >> 4)) & ((unsigned char) 0x0F);
        base64[j++] = trans[(int) current];

        current = ((unsigned char) (data[i + 1] << 2)) & ((unsigned char) 0x3C);
        if (i + 2 >= size) {
            base64[j++] = trans[(int) current];
            base64[j++] = '=';
            break;
        }
        current |= ((unsigned char) (data[i + 2] >> 6)) & ((unsigned char) 0x03);
        base64[j++] = trans[(int) current];

        current = ((unsigned char) data[i + 2]) & ((unsigned char) 0x3F);
        base64[j++] = trans[(int) current];
    }
    base64[j] = '\0';
    return base64;
}

void Base64::Encode(const std::string &in, std::string &out) {
    out.resize(GetEncodeSize((int) in.size()));
    Encode((uint8_t *) in.data(), (int) in.size(), (char *) out.data());
}

int Base64::Decode(const char *base64, uint8_t *data) {
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for (i = 0, j = 0; base64[i] != '\0'; i += 4) {
        memset(temp, 0xFF, sizeof(temp));
        for (k = 0; k < 64; k++) {
            if (trans[k] == base64[i])
                temp[0] = k;
        }
        for (k = 0; k < 64; k++) {
            if (trans[k] == base64[i + 1])
                temp[1] = k;
        }
        for (k = 0; k < 64; k++) {
            if (trans[k] == base64[i + 2])
                temp[2] = k;
        }
        for (k = 0; k < 64; k++) {
            if (trans[k] == base64[i + 3])
                temp[3] = k;
        }

        data[j++] = ((unsigned char) (((unsigned char) (temp[0] << 2)) & 0xFC)) |
                    ((unsigned char) ((unsigned char) (temp[1] >> 4) & 0x03));
        if (base64[i + 2] == '=')
            break;

        data[j++] = ((unsigned char) (((unsigned char) (temp[1] << 4)) & 0xF0)) |
                    ((unsigned char) ((unsigned char) (temp[2] >> 2) & 0x0F));
        if (base64[i + 3] == '=')
            break;

        data[j++] = ((unsigned char) (((unsigned char) (temp[2] << 6)) & 0xF0)) |
                    ((unsigned char) (temp[3] & 0x3F));
    }
    return j;
}

void Base64::Decode(const std::string &in, std::string &out) {
    out.resize(GetDecodeSize(in.data(), (int) in.size()));
    Decode((const char *) in.data(), (uint8_t *) out.data());
}

int Base64::GetDecodeSize(const char *base64, int size) {
    if (size == -1)
        size = (int) strlen(base64);
    int pad = 0;
    for (int i = size - 1; i > 0; i--) {
        if (base64[i] != '=')
            break;
        pad++;
    }
    return size / 4 * 3 - pad;
}
