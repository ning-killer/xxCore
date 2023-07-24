//
// Created by xiong on 2021/6/25.
//

#ifndef EMX_MediaChipQRScan_HPP
#define EMX_MediaChipQRScan_HPP

#include "core/EmxUV.hpp"
#include "MediaServerQRScan.hpp"

namespace Emx {

    class MediaChipQRScan : public MediaServerQRScan {
    public:
        MediaChipQRScan(int32_t chn, MediaServerResource &res)
                : MediaServerQRScan(chn, res), m_started(false), m_mem(-1) {}

        virtual ~MediaChipQRScan() {}

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

    protected:
        virtual void OnQrScanRun(void *arg);

        virtual void OnQrScanComplete(ErrCodeE e, void *arg);

    private:
        bool m_started;
        EuvWork m_work;
        int m_vpssGrp;
        int m_vpssChn;
        int m_mem;
        std::string m_content;
        bool m_got;
    };

}

#endif //EMX_MediaChipQRScan_HPP
