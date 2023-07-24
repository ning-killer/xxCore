//
// Created by xiong on 2020/10/27.
//
#include "MediaServer.hpp"
#include "EmxCore.hpp"

#include "MediaFactory.hpp"
#include "MediaInner.hpp"
#include "MediaConfigs.hpp"
#include <utility>
#include MEDIA_PROFILE_HEADER

using namespace Emx;

MediaServer::MediaServer(const char *dir, MediaFactory *factory) : m_res(dir, factory) {
    m_rpcConnected = false;
    m_topicConnected = false;
    m_mediaCreated = false;
}

void MediaServer::Create(OnCreated cb) {
    // topic和rpc成功启动后的回调
    m_cb = std::move(cb);
    //从Param中获取当前media运行的scene
    Json::Value scene;
    Param param("media/scene");
    if (param.Get(scene) != ErrCodeE::Success) {
        emxloge("get media/scene failed\n");
        return;
    }
    m_res.configs.SetScene(scene["scene"].asCString());
    //获取ringSize,并创建ring buffer
    Json::Value ringSize;
    char path[EMX_MAX_PATH_SIZE];
    snprintf(path, sizeof(path), "%s/server.json", m_res.configs.GetDir());
    EasyJson::Load(path, ringSize);
    m_res.ringSize = ringSize["ringBufferSizeKB"].asInt();
    m_res.ring.Create(MEDIA_RING_BUFFER_KEY, m_res.ringSize * 1024);
    //分配MediaServer所需Msg buffer
    m_res.bufferSize = 4 * 1024;
    m_res.buffer = (char *) malloc(m_res.bufferSize);
    //初始化loop
    m_res.loop.Init(
            "MediaServer",
            [this]() {
                DestroyMedia();
                m_resp.DestroyMsgRpcRep();
                m_res.pub.DestroyMsgTopicPublisherAsync();
            },
            EuvLoop::Default);

    //启动MediaServer topic和rpc服务
    RunTopic();
    RunRpc();

    //启动loop
    m_res.loop.Start(false);
}

void MediaServer::Destroy() { m_res.loop.StopAndDeInit(); }

void MediaServer::RunRpc() {
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_res.loop;
    cfg.clientId = MEDIA_SERVER_MSG_ID;
    cfg.buffer = m_res.buffer;
    cfg.size = m_res.bufferSize;
    cfg.OnConnected = [this]() {
        m_rpcConnected = true;
        if (m_topicConnected) Launcher();
    };
    cfg.OnRecvReq = std::bind(&MediaServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfg);
}

void MediaServer::RunTopic() {
    MsgTopicPublisherAsync::Config cfg = {};
    cfg.loop = &m_res.loop;
    cfg.OnConnected = [this]() {
        printf("%s:%d:MediaServer::RunTopic() connected\n", __FUNCTION__, __LINE__);
        m_topicConnected = true;
        if (m_rpcConnected) Launcher();
    };
    cfg.OnDisConnected = [this]() {
        printf("%s:%d:MediaServer::RunTopic() disconnected\n", __FUNCTION__, __LINE__);
        m_res.pub.DestroyMsgTopicPublisherAsync();
    };
    m_res.pub.CreateMsgTopicPublisherAsync(cfg);
}

ErrCodeE MediaServer::OnRecvReq(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data size=%d\n", module, method, size);
    if ((MediaModE) module == MediaModE::Server) {
        if (method == (uint32_t) MediaServerMethodE::Create) {
            auto e = CreateMedia();
            if (e == ErrCodeE::Success)
                m_res.pub.Publish(MEDIA_TOPIC_SERVER, "CreateMedia", (int) strlen("CreateMedia") + 1);
            return e;
        } else if (method == (uint32_t) MediaServerMethodE::Destroy) {
            auto e = DestroyMedia();
            if (e == ErrCodeE::Success)
                m_res.pub.Publish(MEDIA_TOPIC_SERVER, "DestroyMedia", (int) strlen("DestroyMedia") + 1);
            return e;
        } else if (method == (uint32_t) MediaServerMethodE::SwitchScene) {
            auto e = DestroyMedia();
            if (e != ErrCodeE::Success) return e;
            m_res.pub.Publish(MEDIA_TOPIC_SERVER, "DestroyMedia", (int) strlen("DestroyMedia") + 1);
            //子类继承此方法去实现切换scene时的一些操作
            e = SwitchScene(m_res.configs.GetScene(), data);
            if (e != ErrCodeE::Success) return e;
            e = m_res.configs.SetScene(data);
            if (e == ErrCodeE::Success) {
                Json::Value json;
                Param param("media/scene");
                param.Get(json);
                json["scene"] = data;
                return param.Set(json);
            }
            e = CreateMedia();
            if (e != ErrCodeE::Success) return e;
            m_res.pub.Publish(MEDIA_TOPIC_SERVER, "CreateMedia", (int) strlen("CreateMedia") + 1);
            return ErrCodeE::Success;
        } else
            return OnRecvReqSub(module, method, data, size, out);
    } else {
        if (!m_mediaCreated) return ErrCodeE::ResNotAvailable;
        switch ((MediaModE) module) {
            case MediaModE::System:
                return MediaServerSystem::OnRecvReq(module, method, data, size, out);
            case MediaModE::ISP:
                return MediaServerISP::OnRecvReq(module, method, data, size, out);
            case MediaModE::Venc:
                return MediaServerVenc::OnRecvReq(module, method, data, size, out);
            case MediaModE::Aenc:
                return MediaServerAenc::OnRecvReq(module, method, data, size, out);
            case MediaModE::Adec:
                return MediaServerAdec::OnRecvReq(module, method, data, size, out);
            case MediaModE::Snap:
                return MediaServerSnap::OnRecvReq(module, method, data, size, out);
            case MediaModE::Osd:
                return MediaServerOSD::OnRecvReq(module, method, data, size, out);
            case MediaModE::QrScan:
                return MediaServerQRScan::OnRecvReq(module, method, data, size, out);
            case MediaModE::Ai:
                return MediaServerAi::OnRecvReq(module, method, data, size, out);
            case MediaModE::Vi:
                return MediaServerVi::OnRecvReq(module, method, data, size, out);
            default:
                return OnRecvReqSub(module, method, data, size, out);
        }
    }
}

ErrCodeE MediaServer::OnRecvReqSub(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data size=%d\n", module, method, size);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServer::CreateMedia() {
    if (!m_mediaCreated) {
        if (MediaServerSystem::Create(m_res) != ErrCodeE::Success)
            return ErrCodeE::Failure;
        MediaServerVi::Create(m_res);
        MediaServerISP::Create(m_res);
        MediaServerVenc::Create(m_res);
        MediaServerAenc::Create(m_res);
        MediaServerAdec::Create(m_res);
        MediaServerSnap::Create(m_res);
        MediaServerOSD::Create(m_res);
        MediaServerQRScan::Create(m_res);
        MediaServerAi::Create(m_res);

        m_mediaCreated = true;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaServer::DestroyMedia() {
    if (m_mediaCreated) {
        MediaServerAi::Destroy();
        MediaServerQRScan::Destroy();
        MediaServerOSD::Destroy();
        MediaServerSnap::Destroy();
        MediaServerAdec::Destroy();
        MediaServerAenc::Destroy();
        MediaServerVenc::Destroy();
        MediaServerISP::Destroy();
        MediaServerVi::Destroy();
        MediaServerSystem::Destroy();
        m_mediaCreated = false;
    }
    return ErrCodeE::Success;
}

void MediaServer::Launcher() {
    //通过一个定时器立即创建Media的所有组件，system/venc/aenc等
    auto launcher = new EuvTimer;
    launcher->Create(m_res.loop);
    launcher->Start(0, 0, [this, launcher]() {
        if (CreateMedia() != ErrCodeE::Success)
            return;
        m_cb();
        launcher->Destroy();
        delete launcher;
    });
}
