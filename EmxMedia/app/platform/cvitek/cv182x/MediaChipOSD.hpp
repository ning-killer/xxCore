//
// Created by xiong on 2021/8/10.
//

#ifndef EMX_MediaChipOSD_HPP
#define EMX_MediaChipOSD_HPP

#include "MediaServerOSD.hpp"
#include "sample_comm.h"

namespace Emx {
    class MediaChipOSD : public MediaServerOSD {
    public:
        friend class MediaChipVenc;

        MediaChipOSD(int32_t chn, MediaServerResource &res) :
                MediaServerOSD(chn, res), m_started(false), m_osdIdx(0), m_timeRecord(0) {}

        virtual ~MediaChipOSD() {};
    public:

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const std::vector<std::shared_ptr<Param>> &param) override;

        ErrCodeE GetParam(std::vector<std::shared_ptr<Param>> &param) override;

    protected:
        struct OsdTime {
            OutlineOsd *outline;
            ParamTime *time;
            RGN_HANDLE handle;
        };
        struct OsdText {
            OutlineOsd *outline;
            ParamText *text;
            RGN_HANDLE handle;
        };

        struct OsdImage {
            ParamImage *image;
            RGN_HANDLE handle;
        };

        virtual ErrCodeE Config();

        virtual ErrCodeE CreateTime();

        virtual void DestroyTime();

        virtual ErrCodeE CreateText();

        virtual void DestroyText();

        virtual ErrCodeE CreateImage();

        virtual void DestroyImage();

        virtual ErrCodeE RegionSet(RGN_HANDLE handle,
                                   int x, int y, int w, int h,
                                   PIXEL_FORMAT_E pixFmt, int layer, bool show);

        virtual ErrCodeE ImageToCanvas(OsdImage &image);

        // virtual bool ParamFastConfig(const std::vector<std::shared_ptr<Param>> &param);

        // virtual ErrCodeE RegionShow(RGN_HANDLE handle, bool show);

    protected:
        bool m_started;
        VPSS_GRP m_vpssGrp;
        VPSS_CHN m_vpssChn;
        int m_chnWidth;
        int m_chnHeight;
        int m_osdIdx;
        std::list<OsdTime> m_osdTime;
        std::list<OsdText> m_osdText;
        std::list<OsdImage> m_osdImage;
        EuvTimer m_timer;
        time_t m_timeRecord;
    };
}


#endif //EMX_MediaChipOSD_HPP
