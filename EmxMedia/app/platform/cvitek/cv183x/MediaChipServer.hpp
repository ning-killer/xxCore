#ifndef EMX_MediaChipServer_HPP
#define EMX_MediaChipServer_HPP

#include "MediaServer.hpp"

namespace Emx {

    class MediaChipServer : public MediaServer {
    public:
        MediaChipServer(const char *dir, MediaFactory *factory) :
                MediaServer(dir, factory) {}

        ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) override;
        virtual ~MediaChipServer() {};
    };

}

#endif //EMX_MediaChipServer_HPP
