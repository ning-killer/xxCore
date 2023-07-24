//
// Created by xiong on 2021/5/28.
//

#ifndef EMX_MediaServerSnap_HPP
#define EMX_MediaServerSnap_HPP

#include "media/MediaSnap.hpp"
#include "MediaServerResource.hpp"


namespace Emx {
    class MediaServerSnap : public MediaSnap {
    public:
        explicit MediaServerSnap(int32_t chn, MediaServerResource &res)
                : MediaSnap(chn), m_res(res) {}

        virtual ~MediaServerSnap() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerSnap *Get(int chn) {
            if ((int)m_array.size() > chn)
                return m_array[chn];
            return nullptr;
        }

        static ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                                  const char *data, int32_t size, std::string &out);

    public:
        //note: the data and the size are not include the chn
        virtual ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out);

        virtual ErrCodeE Start() = 0;

        virtual ErrCodeE Stop() = 0;

        virtual ErrCodeE Trigger() = 0;

    protected:
        static std::vector<MediaServerSnap *> m_array;
        MediaServerResource &m_res;
    };
}

#endif //EMX_MediaServerSnap_HPP
