//
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MEDIASERVERVENC_HPP
#define EMX_MEDIASERVERVENC_HPP

#include "media/MediaVenc.hpp"
#include "MediaServerResource.hpp"

namespace Emx {
    class MediaServerVenc : public MediaVenc {
    public:
        explicit MediaServerVenc(int32_t chn, MediaServerResource &res)
                : MediaVenc(chn), m_res(res) {}

        virtual ~MediaServerVenc() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerVenc *Get(int chn) {
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

        ErrCodeE GetParamRange(Json::Value &range) override;

    protected:
        bool ParamValid(const Param &param);

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

    protected:
        //for the simple override of your subclass
        virtual void StartPre() {};

        virtual void StartPost() {};

        virtual void StopPre() {};

        virtual void StopPost() {};
    protected:
        static std::vector<MediaServerVenc *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };
}


#endif //EMX_MEDIASERVERVENC_HPP
