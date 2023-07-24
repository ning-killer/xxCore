/*
* @Author: xiong
* @Date: 2022/12/7
*/
#include "EmxCore.hpp"

using namespace Emx;

static void usage() {
    printf("usage:DevTools param the/path/of/the/file/to/be/decrypt/or/encrypt\n");
}

void CmdParam(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return;
    }

    const char *file = argv[1];
    if (!File::Exist(file)) {
        printf("%s not exist\n", file);
        return;
    }

    //读取源文件
    std::string data;
    if (File::Read(file, data) != ErrCodeE::Success) {
        printf("read file %s failed\n", file);
        return;
    }

    //判断是需要解密还是加密，如果能成功读取证明当前文件是明文，需要加密，否则需要解密
    Json::Value json;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());
    auto enc = reader->parse(data.data(), data.data() + data.size(), &json, &errs);
    printf("#%.*s#\n", (int) data.size(), data.c_str());
    printf("#%s#\n", errs.data());
    printf("#json.empty()=%d#\n", json.empty());

    //解出秘钥
    uint32_t teaCipher[32] = {
            0xf8729d84, 0x7999d514, 0xab3127cb, 0x3e60561e, 0x9cb50ac6, 0xd62488df, 0x4bb57760, 0x8cd144aa,
            0xf9af5fc1, 0x537634ee, 0x7a75da64, 0x1d0480a0, 0xd24bfc1e, 0x7ee597a6, 0x81535bd4, 0x958417ad,
            0xdaa521e8, 0x0a34bdad, 0x4c1a0398, 0xcddba8d0, 0xcd484810, 0x4470bc80, 0x8f082387, 0x1ec8cd06,
            0xbeadb791, 0xc1898a02, 0x1f92c69c, 0x3f3caf16, 0x03fc7a19, 0x3a0fbd11, 0x9d7edf74, 0x7cbf8afb};
    uint8_t key[16] = {};
    uint8_t iv[16] = {};
    TEA tea;
    tea.Decrypt(teaCipher, key, iv);
    AES_CBC m_aes;
    m_aes.Init(key, iv);

    if (enc) {
        //加密
        Pkcs7Padding::Pad(data);
        m_aes.Encrypt((uint8_t *) data.data(), data.size());
    } else {
        //解密
        m_aes.Decrypt((uint8_t *) data.data(), data.size());
        Pkcs7Padding::UnPad(data);
    }
    //写入数据
    if (File::Write(file, (char *) data.data(), (int) data.size()) != ErrCodeE::Success) {
        printf("write file %s failed\n", file);
    } else {
        printf("%s %s success\n", enc ? "Encrypt" : "Decrypt", file);
    }
}
