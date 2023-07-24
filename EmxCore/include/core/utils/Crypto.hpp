//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Crypto_HPP
#define EMX_Crypto_HPP

#include "core/aes/aes.hpp"
#include <cstdint>
#include <cstring>
#include <string>

namespace Emx {

    //!@brief 用于数据的AES CBC模式的加解密
    class AES_CBC {
    public:
        AES_CBC() { memset(&m_ctx, 0, sizeof(m_ctx)); }

        /*!
         * @brief 初始化AES的上下文
         * @param key [in] 密钥
         * @param iv [in] 初始化向量
         */
        void Init(const uint8_t *key, const uint8_t *iv);

        /*!
         * @brief 对数据进行加密，需要保证数据16字节对齐
         * @param buf [in,out] 存放原始数据以及加密后数据
         * @param length [in] 原始数据长度
         */
        void Encrypt(uint8_t *buf, size_t length);

        /*!
         * @brief 对数据进行解密，需要保证数据16字节对齐
         * @param buf [in,out] 存放加密数据以及解密后的数据
         * @param length [in] 加密数据长度
         */
        void Decrypt(uint8_t *buf, size_t length);

    private:
        struct AES_ctx m_ctx;
    };

    //!@brief 用于AES的key和iv的加解密
    class TEA {
    public:
        TEA();

        /*!
         * @brief 使用aes的key和iv编码出teaKey
         * @param key [in] AES key
         * @param iv [in] AES iv
         * @param tea [out] TEA key
         */
        void Encrypt(const uint8_t key[16], const uint8_t iv[16], uint32_t tea[32]) const;

        /*!
         * @brief 使用teaKey解码出aes的key和iv
         * @param tea [in] TEA key
         * @param key [out] AES key
         * @param iv [out] AES iv
         */
        void Decrypt(const uint32_t tea[32], uint8_t key[16], uint8_t iv[16]) const;

    private:
        uint32_t m_delta;
    };

    /**
     * @brief Pkcs7填充与去填充
     */
    class Pkcs7Padding {
    public:
        /**
         * @brief 填充
         * @param [in,out] data
         */
        static void Pad(std::string &data);

        /**
         * @brief 填充
         * @param [in] in 待填充数据
         * @param [in] inSize 待填充数据大小
         * @param [out] out 填充后数据存放buffer
         * @param [in,out] outSize 填充后数据实际大小
         * @return -1:失败
         *          other:填充后数据实际大小
         */
        static int Pad(uint8_t *in, int inSize, uint8_t *out, int &outSize);

        /**
         * @brief 去掉填充
         * @param [in,out] data
         */
        static void UnPad(std::string &data);

        /**
         * @brief 去掉填充
         * @param [in,out] data 待去填充以及去填充后的数据
         * @param [in,out] size 待去填充以及去填充后的数据大小
         * @return -1:失败
         *          other:填充后数据实际大小
         */
        static int UnPad(uint8_t *data, int &size);
    };
}

#endif //EMX_Crypto_HPP
