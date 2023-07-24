/**
 * @author xiaoxiong
 * @brief
 * @date 2022-03-24
 *
 * @copyright Copyright (c) 2022
 *
 */
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MediaChipVenc_HPP
#define EMX_MediaChipVenc_HPP

#include "MediaServerVenc.hpp"
#include "cvi_comm_venc.h"

namespace Emx {

    class MediaChipVenc : public MediaServerVenc {
    public:
        MediaChipVenc(int32_t chn, MediaServerResource &res)
                : MediaServerVenc(chn, res), m_paramChange(false), m_started(false),m_seq(0) {
            memset(&m_stream, 0, sizeof(m_stream));
        }

        virtual ~MediaChipVenc() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE FlushIDR() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

    protected:
        virtual void StartStream();

        virtual void StopStream();

        virtual void OnStream(void *arg);

        virtual void OnStreamComplete(ErrCodeE e, void *arg);

        virtual ErrCodeE Vpss2Venc();

        virtual ErrCodeE Venc2User();

        virtual void PushFrame();

        virtual void DrawMsgRecv(uint32_t topic, const char *data, int32_t size);

        virtual void DrawRectByJson(VIDEO_FRAME_S &stVFrame);

        virtual ErrCodeE Config();

        virtual ErrCodeE CreateChn(Json::Value &json);

        virtual ErrCodeE SetSingleESBuffer(int size);

        virtual ErrCodeE SetVpssChnAlignment();

        virtual ErrCodeE SetRcParam(Json::Value &json);

        virtual ErrCodeE AttachVbPool(Json::Value &json);

        virtual ErrCodeE SetVideoCodecParam(Json::Value &json);

        virtual ErrCodeE SetRefParam(Json::Value &json);

        virtual void StartOsd();

        virtual void StopOsd();

    protected:
        VI_PIPE m_pipe;
        VPSS_GRP m_vpssGrp;
        VPSS_CHN m_vpssChn;
        VENC_CHN m_vencChn;
        bool m_bind;
        bool m_paramChange;
        VENC_STREAM_S m_stream;
        EuvWork m_work;
        bool m_started;
        MsgTopicSubscriberAsync m_drawSub;
        char m_buffer[4096];
        std::map<std::string, Json::Value> m_drawMsgMap;
        uint32_t m_seq;
    };
}// namespace Emx

#endif// EMX_MediaChipVenc_HPP
