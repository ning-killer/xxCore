#ifndef EMX_MediaChipDouLensVenc_HPP
#define EMX_MediaChipDouLensVenc_HPP

#include <cvi_comm_venc.h>
#include "MediaServerVenc.hpp"
#include <thread>
#include <MediaChipVenc.hpp>

typedef struct _VFrBox {
    VB_BLK blk;
    VB_CAL_CONFIG_S stVbCalConfig;
    VIDEO_FRAME_INFO_S stVideoFrame;
    SIZE_S stSize;
} VFrBox;
namespace Emx {

    class MediaChipDouLensVenc : public MediaChipVenc {
    public:
        MediaChipDouLensVenc(int32_t chn, MediaServerResource &res)
                : MediaChipVenc(chn, res) {
            memset(&m_vfboxOut, 0, sizeof(VFrBox));
            memset(&m_stVideoFrame0, 0, sizeof(VIDEO_FRAME_INFO_S));
            memset(&m_stVideoFrame1, 0, sizeof(VIDEO_FRAME_INFO_S));
        }
        virtual ~MediaChipDouLensVenc() {};
    public:
        void PushFrame() override;
        void StartStream() override;
        void StopStream() override;
    protected:
        virtual ErrCodeE Vi2Venc();
        virtual ErrCodeE Stitch2Venc();
        virtual void OnNormalStream(void *arg);
        virtual void OnStitchStream(void *arg);
    private:
        VFrBox m_vfboxOut;
        std::thread m_pthread;
        VIDEO_FRAME_INFO_S m_stVideoFrame0, m_stVideoFrame1;
    };
}// namespace Emx

#endif// EMX_MediaChipVenc_HPP
