/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_Resource_HPP
#define EMX_Resource_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Resource {
    public:
        EuvLoop loop;
        char *buffer;
        int bufferSize;
    };

}

#endif //EMX_Resource_HPP
