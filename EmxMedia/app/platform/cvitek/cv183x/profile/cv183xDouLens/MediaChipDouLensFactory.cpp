#include <MediaServerVi.hpp>
#include <MediaChipVi.hpp>
#include "MediaChipDouLensFactory.hpp"
#include "MediaChipServer.hpp"
#include "MediaChipISP.hpp"
#include "MediaChipDouLensVenc.hpp"
#include "MediaChipAenc.hpp"
#include "MediaChipAdec.hpp"
#include "MediaChipSnap.hpp"
#include "MediaChipQRScan.hpp"
#include "MediaChipOSD.hpp"
#include "MediaChipAi.hpp"
#include "MediaChipDouLensSystem.hpp"
#include "MediaChipDouLensServer.hpp"

using namespace Emx;

MediaServer *MediaChipDouLensFactory::MakeServer(const char *dir, MediaFactory *factory) {
    return new MediaChipDouLensServer(dir, factory);
}

MediaServerSystem *MediaChipDouLensFactory::MakeSystem(MediaServerResource &res) {
    return new MediaChipDouLensSystem(res);
}

MediaServerVi *MediaChipDouLensFactory::MakeVi(int32_t chn, MediaServerResource &res) {
    return new MediaChipVi(chn, res);
}

MediaServerISP *MediaChipDouLensFactory::MakeISP(int32_t chn, MediaServerResource &res) {
    return new MediaChipISP(chn, res);
}

MediaServerVenc *MediaChipDouLensFactory::MakeVenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipDouLensVenc(chn, res);
}

MediaServerAenc *MediaChipDouLensFactory::MakeAenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipAenc(chn, res);
}

MediaServerAdec *MediaChipDouLensFactory::MakeAdec(int32_t chn, MediaServerResource &res) {
    return new MediaChipAdec(chn, res);
}

MediaServerSnap *MediaChipDouLensFactory::MakeSnap(int32_t chn, MediaServerResource &res) {
    return new MediaChipSnap(chn, res);
}

MediaServerQRScan *MediaChipDouLensFactory::MakeQRScan(int32_t chn, MediaServerResource &res) {
    return new MediaChipQRScan(chn, res);
}

MediaServerOSD *MediaChipDouLensFactory::MakeOSD(int32_t chn, MediaServerResource &res) {
    return new MediaChipOSD(chn, res);
}

MediaServerAi *MediaChipDouLensFactory::MakeAi(int32_t chn, MediaServerResource &res) {
    return new MediaChipAi(chn, res);
}
