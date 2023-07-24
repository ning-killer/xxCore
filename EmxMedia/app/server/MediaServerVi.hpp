/*
* @Author: xiong
* @Date: 2023/4/3
*/

#ifndef EMX_MediaServerVi_HPP
#define EMX_MediaServerVi_HPP

#include "media/MediaVi.hpp"
#include "MediaServerResource.hpp"

namespace Emx {

    class MediaServerVi : public MediaVi {
    public:
        explicit MediaServerVi(int32_t chn, MediaServerResource &res)
                : MediaVi(chn), m_res(res) {}

        virtual ~MediaServerVi() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerVi *Get(int chn) {
            if ((int) m_array.size() > chn)
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
        static std::vector<MediaServerVi *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };

}

#endif //EMX_MediaServerVi_HPP
