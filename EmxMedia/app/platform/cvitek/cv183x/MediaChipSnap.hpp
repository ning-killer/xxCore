#ifndef EMX_MediaChipSnap_HPP
#define EMX_MediaChipSnap_HPP

#include "MediaServerSnap.hpp"
#include "cvi_comm_venc.h"

namespace Emx {

    class MediaChipSnap : public MediaServerSnap {
    public:
        MediaChipSnap(int32_t chn, MediaServerResource &res)
                : MediaServerSnap(chn, res), m_started(false) {}

        virtual ~MediaChipSnap() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE Trigger() override;

    protected:
        virtual void OnNormal(void *arg);

        virtual void OnStitch(void *arg);

        virtual void OnStitchComplete(ErrCodeE e, void *arg);

        virtual void OnNormalComplete(ErrCodeE e, void *arg);

        virtual ErrCodeE Vi2Venc();

        virtual ErrCodeE Vpss2Venc();

        virtual ErrCodeE Venc2User(VENC_STREAM_S &stream);

        virtual ErrCodeE PushFrame(VENC_STREAM_S &stream);

    protected:

        VI_CHN m_viChn;
        VI_PIPE m_viPipe;
        int32_t m_vpssGrp;
        int32_t m_vpssChn;
        int32_t m_vencChn;
        bool m_bind;
        bool m_started;
        bool m_isSnapping;
        EuvWork m_work;
        VENC_STREAM_S m_stream;

    };

}

#endif //EMX_MediaChipSnap_HPP
