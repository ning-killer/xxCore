//
// Created by xiong on 2021/4/23.
//

#ifndef EMX_MediaChipServer_HPP
#define EMX_MediaChipServer_HPP

#include "MediaServer.hpp"

namespace Emx {

    class MediaChipServer : public MediaServer {
    public:
        MediaChipServer(const char *dir, MediaFactory *factory) :
                MediaServer(dir, factory) {}

        virtual ~MediaChipServer() {};
    };

}

#endif //EMX_MediaChipServer_HPP
