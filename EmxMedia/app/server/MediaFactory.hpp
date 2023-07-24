//
// Created by xiong on 2020/10/30.
//

#ifndef EMX_MEDIASERVERFACTORY_HPP
#define EMX_MEDIASERVERFACTORY_HPP

#include "core/EmxUV.hpp"
#include "core/EmxMsg.hpp"
#include "MediaServerResource.hpp"
#include "MediaServer.hpp"
#include "MediaServerSystem.hpp"
#include "MediaServerISP.hpp"
#include "MediaServerVenc.hpp"
#include "MediaServerAenc.hpp"
#include "MediaServerAdec.hpp"
#include "MediaServerSnap.hpp"
#include "MediaServerQRScan.hpp"
#include "MediaServerOSD.hpp"
#include "MediaServerAi.hpp"
#include "MediaServerVi.hpp"

namespace Emx {
    class MediaFactory {
    public:
        MediaFactory() {}

        virtual ~MediaFactory() {};

        virtual MediaServer *MakeServer(const char *dir, MediaFactory *factory) = 0;

        virtual MediaServerSystem *MakeSystem(MediaServerResource &res) = 0;

        virtual MediaServerISP *MakeISP(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerVenc *MakeVenc(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerAenc *MakeAenc(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerAdec *MakeAdec(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerSnap *MakeSnap(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerQRScan *MakeQRScan(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerOSD *MakeOSD(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerAi *MakeAi(int32_t chn, MediaServerResource &res) = 0;

        virtual MediaServerVi *MakeVi(int32_t chn, MediaServerResource &res) = 0;
    };
}


#endif //EMX_MEDIASERVERFACTORY_HPP
