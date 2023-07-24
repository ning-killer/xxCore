/*
 * @Author: xiong
 * @Date: 2022/9/5
*/

#ifndef EMX_Emmc_HPP
#define EMX_Emmc_HPP

#include "FlashInf.hpp"

namespace Emx {
    class Emmc : public FlashInf {
    public:
        ErrCodeE Erase(int64_t start, int64_t size) override;
    };
};

#endif //EMX_Emmc_HPP
