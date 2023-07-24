/*
* @Author: xiong
* @Date: 2023/4/3
*/

#include "json/json.h"
#include "media/MediaClientVi.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;


ErrCodeE MediaClientVi::SetParam(const MediaVi::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Vi);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientVi::GetParam(MediaVi::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Vi);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientVi::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Vi);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientVi::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Vi);
    return p.GetChnNum(num);
}