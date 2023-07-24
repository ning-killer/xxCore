/*
* @Author: xiong
* @Date: 2023/4/3
*/

#ifndef EMX_MediaChipVi_HPP
#define EMX_MediaChipVi_HPP

#include "MediaServerVi.hpp"
#include "cvi_comm_vi.h"

namespace Emx {

    class MediaChipVi : public MediaServerVi {
    public:
        MediaChipVi(int32_t chn, MediaServerResource &res)
                : MediaServerVi(chn, res), m_started(false) {}

        virtual ~MediaChipVi() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

    protected:
        VI_PIPE m_pipe;
        VI_CHN m_viChn;
        bool m_originFlip;
        bool m_originMirror;
        bool m_started;
    };
}

#endif //EMX_MediaChipVi_HPP
