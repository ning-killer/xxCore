//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_MD5_HPP
#define EMX_MD5_HPP

#include <string>

namespace Emx {
    //!@brief 计算MD5
    class MD5 {
    public:
        typedef unsigned int size_type; // must be 32bit

        //!初始化MD5上下文
        MD5();

        //!使用特定数据初始化MD5上下文
        MD5(const std::string &text);

        /*!
         * @brief 使用新的数据更新MD5
         * @param buf [in]数据指针
         * @param length [in]数据大小
         */
        void update(const unsigned char *buf, size_type length);

        /*!
         * @brief 使用新的数据更新MD5
         * @param buf [in]数据指针
         * @param length [in]数据大小
         */
        void update(const char *buf, size_type length);

        //!生成最终MD5
        MD5 &finalize();

        //!MD5二进制数据转字符串输出
        std::string hexdigest() const;

        friend std::ostream &operator<<(std::ostream &, MD5 md5);

    private:
        void init();

        typedef unsigned char uint1; //  8bit
        typedef unsigned int uint4;  // 32bit
        enum {
            blocksize = 64
        }; // VC6 won't eat a const static int here

        void transform(const uint1 block[blocksize]);

        static void decode(uint4 output[], const uint1 input[], size_type len);

        static void encode(uint1 output[], const uint4 input[], size_type len);

        bool finalized;
        uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
        uint4 count[2];   // 64bit counter for number of bits (lo, hi)
        uint4 state[4];   // digest so far
        uint1 digest[16]; // the result

        // low level logic operations
        static inline uint4 F(uint4 x, uint4 y, uint4 z);

        static inline uint4 G(uint4 x, uint4 y, uint4 z);

        static inline uint4 H(uint4 x, uint4 y, uint4 z);

        static inline uint4 I(uint4 x, uint4 y, uint4 z);

        static inline uint4 rotate_left(uint4 x, int n);

        static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);

        static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);

        static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);

        static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
    };

}

#endif //EMX_MD5_HPP
