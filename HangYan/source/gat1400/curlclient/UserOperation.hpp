/**
 * @file UserOperation.hpp
 * @author hening
 * @brief 用户操作类，提供curl异步上传用户自定义控制
 * @date 2023-7-3
 */

#ifndef EMX_USEROPERATION_HPP
#define EMX_USEROPERATION_HPP

#include "EmxCore.hpp"

namespace Emx {

    //!@brief 用户操作类
    class UserOperation {
      public:
        virtual ~UserOperation() { }
        virtual bool isUploadOk(const std::string &result) = 0;
    };

    class Gat1400FaceUploadUserOpera : public UserOperation {
      public:
        bool isUploadOk(const std::string &result) override;
    };
}

#endif //EMX_USEROPERATION_HPP
