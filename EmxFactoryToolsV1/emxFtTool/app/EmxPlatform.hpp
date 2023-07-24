//
// Created by xiong on 2021/5/21.
//

#ifndef EMXFCTOOLSV1_EMXPlatform_HPP
#define EMXFCTOOLSV1_EMXPlatform_HPP

#include "EmxCore.hpp"

/**
 * @brief 平台信息
 * 
 */
namespace Emx {

    class EmxPlatform {
    public:
        EmxPlatform(){}

        enum class PlatE : uint16_t {
           EHJQV1,    //和家亲1.0 ---> HJQV1
           EHJQV2,    //和家亲2.0 ---> HJQV2
           E21CN,     //电信21CN  ---> 21CN
           ETY,       //涂鸦      --->
           ETYKJ,     //天翼看店--->
           ECAMTELLS, //CAMTELLS
           EYY,       //云眼--->
           EMJ,       //电信魔镜--->
           EUNKNOWN,  //未知平台--->
        };   

        /**
         * @brief 根据传入的字符串获取平台枚举值
         * 
         * @param platStr 
         * @return PlatE 
         */
        static EmxPlatform::PlatE GetPlat(const std::string platStr);
    };
}

#endif //EMXFCTOOLSV1_EMXPlatform_HPP
