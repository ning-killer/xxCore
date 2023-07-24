//
// Created by xiong on 2021/6/1.
//

#include "EmxCore.hpp"
#include "SDCardServer.hpp"
#include "SDCardInner.hpp"

using namespace Emx;

void SDCardServer::Create(OnCreated cb, bool newThread) {
    m_cb = std::move(cb);
    auto path = new std::string(Misc::GetAppConfigsDir());
    path->append("/sdcard.json");
    if (!File::Exist(path->c_str())) {
        printf("%s not exist\n", path->c_str());
        delete path;
        return;
    }
    if (EasyJson::Load(path->c_str(), m_cfg) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path->c_str());
        delete path;
        return;
    }
    delete path;
    m_res.buffer = m_msgBuf;
    m_res.size = sizeof(m_msgBuf);
    m_res.loop.Init("SDCardServer", [this]() {
        m_res.pub.DestroyMsgTopicPublisherAsync();
        m_launcher.Destroy();
        m_resp.DestroyMsgRpcRep();
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    //receive and process RPC request from client
    m_respConnected = false;
    MsgRpcRep::Config cfgRep = {};
    cfgRep.loop = &m_res.loop;
    cfgRep.clientId = SDCARD_RESP_ID;
    cfgRep.buffer = m_msgBuf;
    cfgRep.size = MaxSdcardBufferSize;
    cfgRep.OnConnected = [this]() {
        m_respConnected = true;
        if (m_pubConnected && m_cb) {
            m_cb();
        }
    };
    cfgRep.OnDisConnected = []() {};
    cfgRep.OnRecvReq = std::bind(&SDCardServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfgRep);

    //publish link stat
    m_pubConnected = false;
    MsgTopicPublisherAsync::Config cfgPub = {};
    cfgPub.loop = &m_res.loop;
    cfgPub.OnConnected = [this]() {
        m_pubConnected = true;
        if (m_respConnected && m_cb) {
            m_cb();
        }
    };
    m_res.pub.CreateMsgTopicPublisherAsync(cfgPub);
    m_launcher.Create(m_res.loop);
    m_launcher.Start(0, 0, std::bind(&SDCardServer::OnCreateDev, this));
    emxlogd("SDCardServer newThread(%d)\n",newThread);
    m_res.loop.Start(newThread);
}

void SDCardServer::Destroy() {
    for (auto e:m_sdcardList) {
        e->Destroy();
        delete e;
    }
    m_sdcardList.clear();
    m_res.loop.StopAndDeInit();
}

void SDCardServer::OnCreateDev() {
    for (int i = 0; i < (int) m_cfg.size(); i++) {
        auto dev = new SDCardDev(m_res, m_cfg[i], i);
        dev->Create();
        m_sdcardList.push_back(dev);
    }
}

ErrCodeE SDCardServer::OnRecvReq(uint32_t module, uint32_t method,
                                 const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data#%s# size=%d\n", module, method, size ? data : " ", size);
    int32_t chn;
    memcpy(&chn, data, sizeof(chn));
    if (chn >= (int32_t) m_sdcardList.size()) {
        emxloge("chn %d not exists\n", chn);
        return ErrCodeE::ResNotExist;
    }
    data += sizeof(chn);
    size -= sizeof(chn);
    if (method == (uint32_t) SDCardMethodE::GetChnNum) {
        int chnNum = (int) m_sdcardList.size();
        out.assign((char *) &chnNum, sizeof(chnNum));
        return ErrCodeE::Success;
    } else {
        return m_sdcardList[chn]->OnRecvReq(module, method, data, size, out);
    }
}

