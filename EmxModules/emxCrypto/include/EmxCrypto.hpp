/*
 * @Author: xiong
 * @Date: 2023/2/4
*/

#ifndef EMX_EmxCrypto_HPP
#define EMX_EmxCrypto_HPP

#include "EmxCore.hpp"

namespace Emx {
    class EmxCrypto {
    public:
        /**
         * @brief RSA公钥解密
         * @param pubKeyPath [in] 密钥文件路径
         * @param src [in] 原始字符串
         * @param dst [out] 目标字符串
         * @return ErrCodeE
         * @note Mbedtls建议使用支持传入解密buffer大小的接口,该接口输入string不能控制解密buffer大小，内部默认最大支持512字节
         */
        static ErrCodeE RsaPubKeyDecrypt(const std::string &pubKeyPath, const std::string &src, std::string &dst) {
            return RsaPubKeyDecrypt(pubKeyPath.data(), src.data(), src.size(), dst);
        }

        /**
         * @brief RSA公钥解密
         * @param pubKeyPath [in] 密钥文件路径
         * @param src [in] 原始字符串
         * @param srcSize [in] 原始字符串长度
         * @param dst [out] 目标字符串
         * @return ErrCodeE
         * @note Mbedtls建议使用支持传入解密buffer大小的接口,该接口输入string不能控制解密buffer大小，内部默认最大支持512字节
         */
        static ErrCodeE RsaPubKeyDecrypt(const char *pubKeyPath, const char *src, int srcSize, std::string &dst);

        /**
         * @brief RSA公钥解密
         * @param pubKeyPath [in] 密钥文件路径
         * @param src [in] 原始字符串
         * @param dst [out] 目标字符串buffer
         * @param dst_maxlen [out] 目标字符串buffer大小
         * @return ErrCodeE
         * @note Mbedtls建议使用优先使用该接口，Openssl不支持该接口
         */
        static ErrCodeE RsaPubKeyDecrypt(const std::string &pubKeyPath, const std::string &src,  unsigned char *dst, const int dst_maxlen);
    };
};

#endif //EMX_EmxCrypto_HPP