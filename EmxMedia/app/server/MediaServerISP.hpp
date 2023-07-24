//
// Created by xiong on 2021/4/23.
//

#ifndef EMX_MediaServerISP_HPP
#define EMX_MediaServerISP_HPP

#include "media/MediaISP.hpp"
#include "MediaServerResource.hpp"

namespace Emx {
    class MediaFactory;

    class MediaServerISP : public MediaISP {
    public:
        explicit MediaServerISP(int32_t chn, MediaServerResource &res)
                : MediaISP(chn), m_res(res) {}

        virtual ~MediaServerISP() {};

        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerISP *Get(int chn) {
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
        static std::vector<MediaServerISP *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };

}

#endif //EMX_MediaServerISP_HPP
