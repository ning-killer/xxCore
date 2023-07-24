/*
* @Author: xiong
* @Date: 2023/2/7
*/
#include "RecordStdServer.hpp"
#include "EmxCore.hpp"
#include "RecordStdInner.hpp"

using namespace Emx;

void RecordStdServer::Create(bool newThread) {
    if (LoadParam() != ErrCodeE::Success)
        return;
    m_res.loop.Init("RecordStdServer", [this]() {
        m_resp.DestroyMsgRpcRep();
        m_sdcard.event->Destroy();
        delete m_sdcard.event;
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    //receive and process RPC request from client
    MsgRpcRep::Config cfgRep = {};
    cfgRep.loop = &m_res.loop;
    cfgRep.clientId = RECORD_STD_MSG_ID;
    cfgRep.buffer = m_res.buffer;
    cfgRep.size = sizeof(m_res.buffer);
    cfgRep.OnConnected = []() {};
    cfgRep.OnDisConnected = []() {};
    cfgRep.OnRecvReq = std::bind(&RecordStdServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfgRep);
    //先将sdcard的状态配置为无效
    m_sdcard.normal = false;
    //创建SDCardClientEvent来监听sd的状态改变事件
    m_sdcard.event = new SDCardClientEvent(m_res.loop,
                                           m_res.buffer, sizeof(m_res.buffer),
                                           std::bind(&RecordStdServer::OnSDCardInfoEvent, this, ph_1, ph_2));
    // sdcard event 创建成功后会收到OnSDCardInfoEvent的异步回调,由此开始循环创建录像文件
    m_sdcard.event->Create();
    m_res.loop.Start(newThread);

}


void RecordStdServer::Destroy() {
    m_res.loop.StopAndDeInit();
}


ErrCodeE RecordStdServer::OnRecvReq(uint32_t module, uint32_t method,
                                    const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data#%s# size=%d\n", module, method, size ? data : " ", size);

    switch ((RecordStdModuleE) module) {
        case RecordStdModuleE::RecordStd: {
            if (method == (uint32_t) RecordStdMethodE::GetParam) {
                m_res.param->Serialize(out);
                return ErrCodeE::Success;
            } else if (method == (uint32_t) RecordStdMethodE::SetParam) {
                auto param = std::make_shared<RecordStd::Param>();
                param->Parse(data, size);
                // 存储路径发生改变需要重新构造文件列表，需要重启record和snap
                if (strcmp(param->path, m_res.param->path) != 0) {
                    m_record.Destroy();
                    m_snap.Destroy();
                    m_res.param = param;
                    m_record.Create(&m_res);
                    m_snap.Create(&m_res);
                } else {
                    m_record.Stop();
                    m_snap.Stop();
                    m_res.param = param;
                    m_record.Start();
                    m_snap.Start();
                }
                SaveParam();
                return ErrCodeE::Success;
            } else {
                return ErrCodeE::IllegalParam;
            }
        }
        case RecordStdModuleE::RecordStdRecord:
            return m_record.OnRecvReq(module, method, data, size, out);
        case RecordStdModuleE::RecordStdSnap:
            return m_snap.OnRecvReq(module, method, data, size, out);
        default:
            return ErrCodeE::IllegalParam;
    }
}

ErrCodeE RecordStdServer::LoadParam() {
    ::Param param("recordStd");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load recordStd param failed\n");
        return ErrCodeE::Failure;
    }
    m_res.param = std::make_shared<RecordStd::Param>();
    m_res.param->Parse(json);
    return ErrCodeE::Success;
}

ErrCodeE RecordStdServer::SaveParam() {
    ::Param param("recordStd");
    Json::Value json;
    m_res.param->Serialize(json);
    if (param.Set(json) != ErrCodeE::Success) {
        emxloge("save recordStd param failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void RecordStdServer::OnSDCardInfoEvent(int chn, SDCard::Info &info) {
    //确认存储路径所在SD卡通道号
    if (strncmp(info.mountPoint, m_res.param->path, strlen(info.mountPoint)) == 0) {
        m_res.sdcardChn = chn;
    } else {
        return;
    }
    bool normal = info.stat == SDCard::StatE::Normal;
    if (m_sdcard.normal == normal)
        return;
    m_sdcard.normal = normal;
    emxlogi("stat normal = %s\n", normal ? "true" : "false");
    if (normal) {
        m_record.Create(&m_res);
        m_snap.Create(&m_res);
    } else {
        m_snap.Destroy();
        m_record.Destroy();
    }
}

