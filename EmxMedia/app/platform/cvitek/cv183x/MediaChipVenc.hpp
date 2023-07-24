#ifndef EMX_MediaChipVenc_HPP
#define EMX_MediaChipVenc_HPP

#include <cvi_comm_venc.h>
#include "MediaServerVenc.hpp"
#include <thread>
namespace Emx {

    class MediaChipVenc : public MediaServerVenc {

    public:

        MediaChipVenc(int32_t chn, MediaServerResource &res)
                : MediaServerVenc(chn, res),m_started(false), m_paramChange(false), m_seq(0){
            memset(&m_stream, 0, sizeof(m_stream));
           ;
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

        virtual ErrCodeE AttachVbPool(Json::Value &json);

        virtual ErrCodeE SetSingleESBuffer(int size);

        virtual ErrCodeE SetVpssChnAlignment();

        virtual ErrCodeE SetRcParam(Json::Value &json);

        virtual ErrCodeE SetRefParam(Json::Value &json);

        virtual ErrCodeE SetVideoCodecParam(Json::Value &json);

        void StartOsd();

        void StopOsd();

    protected:
        VI_CHN m_viChn;
        VI_PIPE m_pipe;
        VPSS_GRP m_vpssGrp;
        VPSS_CHN m_vpssChn;

        bool m_bind;
        bool m_started;
        bool m_paramChange;
        EuvWork m_work;
        uint32_t m_seq;
        VENC_CHN m_vencChn;
        VENC_STREAM_S m_stream;

    private:
        MsgTopicSubscriberAsync m_drawSub;
        char m_buffer[4096];
        std::map<std::string, Json::Value> m_drawMsgMap;
    };
}// namespace Emx

#endif// EMX_MediaChipVenc_HPP
