//
// Created by cx on 2023/4/21.
//

#ifndef Emx_MediaChipDouLensServer_H
#define Emx_MediaChipDouLensServer_H
#include "MediaChipServer.hpp"
namespace Emx {
    class MediaChipDouLensServer : public MediaChipServer {
    public:
        MediaChipDouLensServer(const char *dir, MediaFactory *factory) : MediaChipServer(dir,factory){}

        virtual ~MediaChipDouLensServer() {}
        ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) override;
    public:
    };
}
#endif //Emx_MediaChipDouLensServer_H
