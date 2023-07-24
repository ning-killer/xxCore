//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Base64_HPP
#define EMX_Base64_HPP

#include <string>

namespace Emx {

    //!@brief Base64编解码
    class Base64 {
    public:
        /**
         * @brief base64编码
         * @param in [in] 需要编码的原始数据
         * @param out [out] 编码后的数据
         */
        static void Encode(const std::string &in, std::string &out);

        /**
         * @brief base64编码
         * @param data [in] 需要编码的原始数据
         * @param size [in] 需要编码的原始数据长度
         * @param base64 [out] 承接编码后数据的内存地址，需要调用者保证空间够用
         *                      base64加密所需长度计算：输出长度=[输入长度/3]*4 ，[]代表上取整
         * @return 编码后数据的内存地址
         */
        static char *Encode(const uint8_t *data, int size, char *base64);

        /**
         * @brief 获取base64编码所需空间大小
         * @param size [in] 待编码数据大小
         * @return 编码后数据大小
         */
        static int GetEncodeSize(int size) {
            return ((size / 3) + (int) (size % 3 != 0)) * 4 + 1;// + 1 for '/0'
        }

        /**
         * @brief base64解码
         * @param in [in] 需要解码的数据
         * @param out [out] 解码后的数据
         */
        static void Decode(const std::string &in, std::string &out);

        /**
         * @brief base64解码
         * @param base64 [in] 需要解码的数据
         * @param data [in] 承接解码后数据的内存地址，需要调用者保证解密空间够用
         *                  base64解密所需长度计算：输出长度=输入长度/4*3 ，[]代表上取整
         * @return 解码后原始数据的实际长度
         */
        static int Decode(const char *base64, uint8_t *data);

        /**
         * @brief 获取base64解码所需空间大小
         * @param base64 [in] 待解码数据
         * @param size [in] 待解码数据大小(选填)
         * @return 解码后数据大小
         */
        static int GetDecodeSize(const char *base64, int size = -1);

    private:
        static const char *trans;
    };


}

#endif //EMX_Base64_HPP
