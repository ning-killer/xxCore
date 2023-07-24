/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileHeader_HPP
#define EMX_FileHeader_HPP

#include "EmxCore.hpp"

namespace Emx {

    struct FileHeader {
        static const int HeaderSize = 4 + 4 + 4;
        char magic[4]; //!<固定为'E','P','F','P'
        enum class CmdE : uint32_t {
            FileWrite = 2001,
            FileRead = 2002,
            LogUpload = 3001,
            Update = 4001,
        } cmd;
        uint32_t size;
    };
}


#endif //EMX_FileHeader_HPP
