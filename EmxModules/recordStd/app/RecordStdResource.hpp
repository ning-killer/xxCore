/*
* @Author: xiong
* @Date: 2023/2/27
*/

#ifndef EMX_RecordStdResource_HPP
#define EMX_RecordStdResource_HPP

#include "RecordStd.hpp"

namespace Emx {

    class RecordStdResource {
    public:
        EuvLoop loop;
        char buffer[1024];
        std::shared_ptr<RecordStd::Param> param;
        int sdcardChn;
    };

}

#endif //EMX_RecordStdResource_HPP
