#ifndef EMX_MediaChipDouLensFactory_HPP
#define EMX_MediaChipDouLensFactory_HPP

#include "MediaFactory.hpp"

namespace Emx {
    class MediaChipDouLensFactory : public MediaFactory {
    public:
        MediaServer *MakeServer(const char *dir, MediaFactory *factory) override;

        MediaServerSystem *MakeSystem(MediaServerResource &res) override;

        MediaServerVi *MakeVi(int32_t chn, MediaServerResource &res) override;

        MediaServerVenc *MakeVenc(int32_t chn, MediaServerResource &res) override;

        MediaServerAenc *MakeAenc(int32_t chn, MediaServerResource &res) override;

        MediaServerAdec *MakeAdec(int32_t chn, MediaServerResource &res) override;

        MediaServerISP *MakeISP(int32_t chn, MediaServerResource &res) override;

        MediaServerSnap *MakeSnap(int32_t chn, MediaServerResource &res) override;

        MediaServerQRScan *MakeQRScan(int32_t chn, MediaServerResource &res) override;

        MediaServerOSD *MakeOSD(int32_t chn, MediaServerResource &res) override;

        MediaServerAi *MakeAi(int32_t chn, MediaServerResource &res) override;
    };
}
#endif //EMX_MediaChipFactory_HPP
