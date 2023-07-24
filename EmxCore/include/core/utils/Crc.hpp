//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Crc_HPP
#define EMX_Crc_HPP

#include <cstdint>

namespace Emx {

    //!CRC32校验
    class Crc {
    public:
        /*!
         * @brief 计算CRC32值
         * @param crc [in]CRC32的初始值，可以用来分批次计算，初始请填0
         * @param data [in]需要计算的数据
         * @param size [in]需要计算的数据大小
         * @return CRC32值
         */
        static uint32_t crc32(uint32_t crc, const uint8_t *data, int size);

    private:
        static const uint32_t tab[];
    };

}

#endif //EMX_Crc_HPP
