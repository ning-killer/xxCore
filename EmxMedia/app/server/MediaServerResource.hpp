//
// Created by xiong on 2021/7/9.
//

#ifndef EMX_MediaServerResource_HPP
#define EMX_MediaServerResource_HPP

#include "core/EmxMsg.hpp"
#include "media/MediaRingBuffer.hpp"
#include "MediaConfigs.hpp"

namespace Emx {
    class MediaFactory;

    struct MediaServerResource {
        using MediaCallback = std::function<void()>;

        MediaServerResource(const char *inDir, MediaFactory *inFactory) :
                configs(inDir), factory(inFactory) {
            buffer = nullptr;
            bufferSize = 0;
        }

        EuvLoop loop;
        MsgTopicPublisherAsync pub;
        char *buffer;
        int32_t bufferSize;
        MediaConfigs configs;
        MediaRingBufferWriter ring;
        int32_t ringSize;
        MediaFactory *factory;
    };
}

#endif //EMX_MediaServerResource_HPP

