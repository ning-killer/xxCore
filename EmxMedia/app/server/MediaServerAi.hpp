//
// Created by xiong on 2022/4/27.
//

#ifndef EMX_MediaServerAi_HPP
#define EMX_MediaServerAi_HPP

#include "media/MediaAi.hpp"
#include "MediaServerResource.hpp"

namespace Emx {
    class MediaServerAi : public MediaAi {
    public:
        /**
         * @param chn
         * @param res
         */
        explicit MediaServerAi(int32_t chn, MediaServerResource &res)
        : MediaAi(chn), m_res(res) {}

        virtual ~MediaServerAi() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerAi *Get(int chn) {
            if ((int)m_array.size() > chn)
                return m_array[chn];
            return nullptr;
        }
        /**
         * @brief
         * @param[in]
         * @param[in] method
         * @param[in,out] data
         * @param[in] size
         * @param[out] out
         * @return
         */
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
        static std::vector<MediaServerAi *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };

}

#endif //EMX_MediaServerAi_HPP
