/*
 * @Author: xiong
 * @Date: 2022/8/26
*/

#ifndef EMX_FlashInf_HPP
#define EMX_FlashInf_HPP

#include "EmxCore.hpp"
#include <mtd/mtd-user.h>

namespace Emx {

    class FlashInf {
    public:
        FlashInf() { m_fd = -1; }

        virtual ~FlashInf();

        virtual ErrCodeE Open(const char *dev);

        virtual ErrCodeE Close();

        virtual ErrCodeE Erase(int64_t start, int64_t size);

        virtual ErrCodeE Write(const uint8_t *data, int64_t size);

        virtual ErrCodeE Read(uint8_t *data, int64_t size);

        virtual ErrCodeE Seek(off_t offset, int whence);

        virtual ErrCodeE GetInfo(mtd_info_t &mtd);

    protected:
        int m_fd;
    };
}


#endif //EMX_FlashInf_HPP
