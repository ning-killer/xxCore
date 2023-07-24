//
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MEDIASERVERSYSTEM_HPP
#define EMX_MEDIASERVERSYSTEM_HPP

#include "MediaServerResource.hpp"

namespace Emx {
    class MediaServerSystem {
    public:
        explicit MediaServerSystem(MediaServerResource &res) : m_res(res) {}

        virtual ~MediaServerSystem() {};

        static ErrCodeE Create(MediaServerResource &res);

        static void Destroy();

        static MediaServerSystem *Get() { return m_inst; }

        static ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                                  const char *data, int32_t size, std::string &out);

    public:
        virtual ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out);

        virtual ErrCodeE Start() = 0;

        virtual ErrCodeE Stop() = 0;

    protected:
        //for the simple override of your subclass
        virtual void StartPre(const Json::Value &json) {};

        virtual void StartPost(const Json::Value &json) {};

        virtual void StopPre(const Json::Value &json) {};

        virtual void StopPost(const Json::Value &json) {};
    protected:
        static MediaServerSystem *m_inst;
        MediaServerResource &m_res;
    };

}


#endif //EMX_MEDIASERVERSYSTEM_HPP
