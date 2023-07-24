//
// Created by xiong on 2020/11/23.
//

#ifndef EMX_MEDIASERVERADEC_HPP
#define EMX_MEDIASERVERADEC_HPP

#include "media/MediaAdec.hpp"
#include "MediaServerResource.hpp"


namespace Emx {
    class MediaServerAdec : public MediaAdec {
    public:
        explicit MediaServerAdec(int32_t chn, MediaServerResource &res)
                : MediaAdec(chn), m_res(res) {}

        virtual ~MediaServerAdec() {};
    public:
        static void Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerAdec *Get(int chn) {
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
        static std::vector<MediaServerAdec *> m_array;
        MediaServerResource &m_res;
        Param m_param;
    };
}
#endif //EMX_MEDIASERVERADEC_HPP
