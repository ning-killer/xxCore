//
// Created by xiong on 2021/7/13.
//

#include "MediaChipFactory.hpp"
#include "MediaChipServer.hpp"
#include "MediaChipSystem.hpp"
#include "MediaChipISP.hpp"
#include "MediaChipVenc.hpp"
#include "MediaChipAenc.hpp"
#include "MediaChipAdec.hpp"
#include "MediaChipSnap.hpp"
#include "MediaChipQRScan.hpp"
#include "MediaChipOSD.hpp"
#include "MediaChipAi.hpp"
#include "MediaChipVi.hpp"

using namespace Emx;

MediaServer *MediaChipFactory::MakeServer(const char *dir, MediaFactory *factory) {
    return new MediaChipServer(dir, factory);
}

MediaServerSystem *MediaChipFactory::MakeSystem(MediaServerResource &res) {
    return new MediaChipSystem(res);
}

MediaServerISP *MediaChipFactory::MakeISP(int32_t chn, MediaServerResource &res) {
    return new MediaChipISP(chn, res);
}

MediaServerVenc *MediaChipFactory::MakeVenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipVenc(chn, res);
}

MediaServerAenc *MediaChipFactory::MakeAenc(int32_t chn, MediaServerResource &res) {
    return new MediaChipAenc(chn, res);
}

MediaServerAdec *MediaChipFactory::MakeAdec(int32_t chn, MediaServerResource &res) {
    return new MediaChipAdec(chn, res);
}

MediaServerSnap *MediaChipFactory::MakeSnap(int32_t chn, MediaServerResource &res) {
    return new MediaChipSnap(chn, res);
}

MediaServerQRScan *MediaChipFactory::MakeQRScan(int32_t chn, MediaServerResource &res) {
    return new MediaChipQRScan(chn, res);
}

MediaServerOSD *MediaChipFactory::MakeOSD(int32_t chn, MediaServerResource &res) {
    return new MediaChipOSD(chn, res);
}

MediaServerAi *MediaChipFactory::MakeAi(int32_t chn, MediaServerResource &res) {
    return new MediaChipAi(chn, res);
}

MediaServerVi *MediaChipFactory::MakeVi(int32_t chn, MediaServerResource &res) {
    return new MediaChipVi(chn, res);
}
