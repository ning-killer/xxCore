//
// Created by xiong on 2021/6/1.
//

#ifndef EMX_SDCardServer_HPP
#define EMX_SDCardServer_HPP

#include "SDCard.hpp"
#include "SDCardResource.hpp"
#include "SDCardDev.hpp"

namespace Emx {

    class SDCardServer {
    public:
        SDCardServer() : m_cb(nullptr) {}

        virtual ~SDCardServer() {}

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        void OnCreateDev();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

    private:
        SDCardResource m_res;
        MsgRpcRep m_resp;
        EuvTimer m_launcher;
        static const int MaxSdcardBufferSize = 1024;
        char m_msgBuf[MaxSdcardBufferSize];
        bool m_pubConnected;
        bool m_respConnected;
        Json::Value m_cfg;
        std::vector<SDCardDev *> m_sdcardList;
        OnCreated m_cb;
    };

}

#endif //EMX_SDCardServer_HPP
