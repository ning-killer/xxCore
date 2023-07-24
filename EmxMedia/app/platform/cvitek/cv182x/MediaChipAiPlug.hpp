//
// Created by xiong on 2022/5/11.
//

#ifndef EMX_MediaChipAiPlug_HPP
#define EMX_MediaChipAiPlug_HPP

#include <utility>

#include "EmxCore.hpp"
#include "MediaChipAiHandle.hpp"
#include "MediaServerResource.hpp"
#include "media/MediaAi.hpp"
#include "sample_comm.h"

namespace Emx {

    class MediaChipAiPlug {
    public:
        MediaChipAiPlug(int chn, MediaChipAiHandle *handle, MediaServerResource &res, MediaAi::Param &param) :
                m_chn(chn), m_handle(handle), m_res(res), m_param(param) {}

        virtual ~MediaChipAiPlug() {}

        virtual void Create(Json::Value &cfg) = 0;

        virtual void Destroy() = 0;

        virtual void ConfigAiParam(const MediaAi::Param &param){return;}

        using OnFrameCallBack = std::function<void(VIDEO_FRAME_INFO_S &frame)>;
        using OnCompleteCallBack = std::function<void()>;

        struct CallBack {
            CallBack(int vpssGrp, int vpssChn, OnFrameCallBack cbFrame, OnCompleteCallBack cbComplete) :
                    m_vpssGrp(vpssGrp),
                    m_vpssChn(vpssChn),
                    m_cbFrame(std::move(cbFrame)),
                    m_cbComplete(std::move(cbComplete)) {}

            int m_vpssGrp;
            int m_vpssChn;
            OnFrameCallBack m_cbFrame;
            OnCompleteCallBack m_cbComplete;
        };

        std::list<CallBack> &GetCallBackList() { return m_callBackList; }

    protected:
        int m_chn;
        MediaChipAiHandle *m_handle;
        MediaServerResource &m_res;
        MediaAi::Param &m_param;
        std::list<CallBack> m_callBackList;
    };

}

#endif //EMX_MediaChipAiPlug_HPP
