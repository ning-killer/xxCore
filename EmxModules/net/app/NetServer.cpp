//
// Created by xiong on 2021/3/9.
//
#include "csignal"
#include "EmxCore.hpp"
#include "NetServer.hpp"
#include "NetServerDevFactory.hpp"
#include "NetInner.hpp"

#define NET_SERVER_LOOP_BUFFER_SIZE (4*1024)

using namespace Emx;

void NetServer::Create(OnCreated cb, bool newThread) {
    m_cb = cb;
    snprintf(m_res.netConfigDir, sizeof(m_res.netConfigDir), "%s/net", Misc::GetAppConfigsDir());
    m_res.loop.Init("NetServer", [this]() {
        m_resp.DestroyMsgRpcRep();
        m_res.pub.DestroyMsgTopicPublisherAsync();
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);

    Param param("netServer");
    if (param.Get(m_res.param) != ErrCodeE::Success) {
        emxlogc("get netManager configs failed\n");
        return;
    }

    m_res.buffer = (char *) malloc(NET_SERVER_LOOP_BUFFER_SIZE);
    if (!m_res.buffer) {
        emxlogc("malloc %d failed\n", NET_SERVER_LOOP_BUFFER_SIZE);
        return;
    }
    m_res.bufferSize = NET_SERVER_LOOP_BUFFER_SIZE;

    //receive and process RPC request from net client
//    m_respConnected = false;
    MsgRpcRep::Config cfgRep = {};
    cfgRep.loop = &m_res.loop;
    cfgRep.clientId = NET_SERVER_RESP_ID;
    cfgRep.buffer = m_res.buffer;
    cfgRep.size = m_res.bufferSize;
    cfgRep.OnConnected = [this]() {
        m_respConnected = true;
        if (m_pubConnected && m_cb) {
            m_cb();
        }
        emxlogd("Rpc connected\n");
    };
    cfgRep.OnDisConnected = []() { emxlogd("Rpc disconnected\n"); };
    cfgRep.OnRecvReq = std::bind(&NetServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfgRep);

    //publish link stat of net server
    m_pubConnected = false;
    MsgTopicPublisherAsync::Config cfgPub = {};
    cfgPub.loop = &m_res.loop;
    cfgPub.OnConnected = [this]() {
        m_pubConnected = true;
        if (m_respConnected && m_cb) {
            m_cb();
        }
        emxlogd("Topic connected\n");
    };
    cfgPub.OnDisConnected = [this]() {
        emxlogd("Topic disconnected\n");
        m_res.pub.DestroyMsgTopicPublisherAsync();
    };
    m_res.pub.CreateMsgTopicPublisherAsync(cfgPub);

    m_res.saveParam = std::bind(&NetServer::SaveParam, this);

    m_launcher.Create(m_res.loop);
    m_launcher.Start(0, 0, [this]() {
        m_launcher.Destroy();

        for (auto &e : m_res.param["dev"]) {
            if (e["supported"].asBool()) {
                auto dev = NetServerDevFactory::Make(m_res, e);
                if (dev) {
                    dev->Create();
                    m_mapDev[Net::GetDevType(e["name"].asCString())] = dev;
                    emxlogi("create net device %s\n", e["name"].asCString());
                } else {
                    emxlogc("make %s failed\n", e["name"].asCString());
                }
            }
        }
    });
    emxlogd("netServer newThread(%d)\n",newThread);
    m_res.loop.Start(newThread);
}

void NetServer::Destroy() {
    for (auto &e : m_mapDev) {
        e.second->Destroy();
        delete e.second;
    }
    m_res.loop.StopAndDeInit();
    free(m_res.buffer);
}

ErrCodeE NetServer::OnRecvReq(uint32_t module, uint32_t method,
                              const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data#%s# size=%d\n", module, method, size ? data : " ", size);

    if (module == NET_SERVER_DNS_MODULE_ID) {
        if (method == (uint32_t) NetClientDnsMethodE::GetParam) {
            EasyJson::Serialize(m_res.param["dns"], out);
            return ErrCodeE::Success;
        } else if (method == (uint32_t) NetClientDnsMethodE::SetParam) {
            EasyJson::Parse(data, size, m_res.param["dns"]);
            auto dev = Net::GetDevType(m_res.param["main"].asCString());
            m_mapDev[dev]->DoDns();
            m_res.saveParam();
            return ErrCodeE::Success;
        } else {
            return ErrCodeE::OperationNotSupport;
        }
    } else {
        auto m = (Net::DevE) module;
        if (m == Net::DevE::None) {
            return Proc(method, data, size, out);
        } else if (m_mapDev.count(m)) {
            return m_mapDev[m]->Proc(method, data, size, out);
        } else {
            emxlogt("not support module[%u]\n", module);
            return ErrCodeE::OperationNotSupport;
        }
    }

}

ErrCodeE NetServer::SaveParam() {
    Param param("netServer");
    if (param.Set(m_res.param) != ErrCodeE::Success) {
        emxlogc("set net param failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE NetServer::Proc(uint32_t method, const char *data, int32_t size, std::string &out) {
    switch ((NetMainMethodE) method) {
        case NetMainMethodE::Get: {
            auto dev = Net::GetDevType(m_res.param["main"].asCString());
            out.assign((char *) &dev, sizeof(dev));
            return ErrCodeE::Success;
        }
        case NetMainMethodE::Set: {
            Net::DevE dev;
            memcpy(&dev, data, sizeof(dev));
            if (m_mapDev.count(dev) == 0) {
                return ErrCodeE::ResNotExist;
            }
            if (dev != Net::GetDevType(m_res.param["main"].asCString())) {
                m_res.param["main"] = Net::GetDevName(dev);
                SaveParam();
            }
            return ErrCodeE::Success;
        }
        case NetMainMethodE::IsSupported: {
            Net::DevE dev;
            memcpy(&dev, data, sizeof(dev));
            return m_mapDev.count(dev) ? ErrCodeE::Success : ErrCodeE::ResNotExist;
        }
        default:
            emxloge("main not support method[%u]\n", method);
            return ErrCodeE::OperationNotSupport;
    }
}
