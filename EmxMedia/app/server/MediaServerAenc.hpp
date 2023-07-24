/**
 *
 */
//
// Created by xiong on 2020/11/19.
//

#ifndef EMX_MEDIASERVERAENC_HPP
#define EMX_MEDIASERVERAENC_HPP

#include "media/MediaAenc.hpp"
#include "MediaServerResource.hpp"


namespace Emx {
  class MediaServerAenc : public MediaAenc {
    public:
        /**
         * @param chn
         * @param res
         */
        explicit MediaServerAenc(int32_t chn, MediaServerResource &res)
                : MediaAenc(chn), m_res(res) {}

        virtual ~MediaServerAenc() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerAenc *Get(int chn) {
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
        //for the simple override of your subclass
        virtual void StartPre() {};

        virtual void StartPost() {};

        virtual void StopPre() {};

        virtual void StopPost() {};
    protected:
        static std::vector<MediaServerAenc *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };
}


#endif //EMX_MEDIASERVERAENC_HPP
