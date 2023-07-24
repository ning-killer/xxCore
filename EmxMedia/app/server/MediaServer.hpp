//
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MEDIASERVER_HPP
#define EMX_MEDIASERVER_HPP

#include "MediaServerResource.hpp"
#include "media/MediaTypedef.hpp"
#include <map>

namespace Emx {
    class MediaServer {
    public:
        MediaServer(const char *dir, MediaFactory *factory);

        virtual ~MediaServer() {}

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb);

        void Destroy();

    protected:
        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

        virtual ErrCodeE CreateMedia();

        virtual ErrCodeE DestroyMedia();

        virtual ErrCodeE SwitchScene(const char *curScene, const char *newScene) { return ErrCodeE::Success; }

        virtual ErrCodeE OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out);

    private:
        void RunRpc();

        void RunTopic();

        void Launcher();

    protected:
        MediaServerResource m_res;
        MsgRpcRep m_resp;
        bool m_rpcConnected;
        bool m_topicConnected;
        OnCreated m_cb;
        bool m_mediaCreated;
    };
}// namespace Emx

#endif//EMX_MEDIASERVER_HPP
