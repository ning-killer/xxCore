#include "MediaChipFDPDWithFactory.hpp"
#include "MediaChipServer.hpp"
#include "MediaChipSystem.hpp"
#include "MediaChipISP.hpp"
#include "MediaChipVenc.hpp"
#include "MediaChipAenc.hpp"
#include "MediaChipAdec.hpp"
#include "MediaChipSnap.hpp"
#include "MediaChipQRScan.hpp"
#include "MediaChipOSD.hpp"
#include "MediaChipVi.hpp"
#include "MediaChipFDPFWithAi.hpp"

using namespace Emx;

MediaServer *MediaChipFDPDWithFactory::MakeServer(const char *dir, MediaFactory *factory) {
    return new MediaChipServer(dir, factory);
}

MediaServerSystem *MediaChipFDPDWithFactory::MakeSystem(MediaServerResource &res) {
    return new MediaChipSystem(res);
}

MediaServerISP *MediaChipFDPDWithFactory::MakeISP(int32_t chn, MediaServerResource &res) {
    return new MediaChipISP(chn, res);
}

MediaServerVenc *MediaChipFDPDWithFactory::MakeVenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipVenc(chn, res);
}

MediaServerAenc *MediaChipFDPDWithFactory::MakeAenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipAenc(chn, res);
}

MediaServerAdec *MediaChipFDPDWithFactory::MakeAdec(int32_t chn, MediaServerResource &res) {
    return new MediaChipAdec(chn, res);
}

MediaServerSnap *MediaChipFDPDWithFactory::MakeSnap(int32_t chn, MediaServerResource &res) {
    return new MediaChipSnap(chn, res);
}

MediaServerQRScan *MediaChipFDPDWithFactory::MakeQRScan(int32_t chn, MediaServerResource &res) {
    return new MediaChipQRScan(chn, res);
}

MediaServerOSD *MediaChipFDPDWithFactory::MakeOSD(int32_t chn, MediaServerResource &res) {
    return new MediaChipOSD(chn, res);
}

MediaServerAi *MediaChipFDPDWithFactory::MakeAi(int32_t chn, MediaServerResource &res) {
    return new MediaChipFDPDWithAi(chn, res);
}

MediaServerVi *MediaChipFDPDWithFactory::MakeVi(int32_t chn, MediaServerResource &res) {
    return new MediaChipVi(chn, res);
}
