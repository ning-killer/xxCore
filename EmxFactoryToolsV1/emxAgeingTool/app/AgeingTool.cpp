#include "AgeingTool.hpp"
#include "SDCardClient.hpp"
#include "libconfig.h"
using namespace Emx;

AgeingTool::AgeingTool()
    : m_ageing_ctx(new AgeingCtx(&m_loop)) {
}

AgeingTool::~AgeingTool() {
    if (m_ageing_ctx != nullptr) {
        delete m_ageing_ctx;
        m_ageing_ctx = nullptr;
    }
}

ErrCodeE AgeingTool::Stop() {
    if (m_ageing_ctx != nullptr) {
        m_ageing_ctx->Stop();
    }
    return ErrCodeE::Success;
}

ErrCodeE AgeingTool::Create() {
    ErrCodeE ret = ErrCodeE::Failure;
    do {
        if (m_ageing_ctx == nullptr) {
            emxloge("m_ageing_ctx is nullptr\n");
            break;
        }
        // if (SDCardClient::GetChnNum() < 1) {
        //     emxloge("sdcard chn res not exitst\n");
        //     break;
        // }
        // //note: 默认取第一通道sdcard路径作为老化工具配置检测
        // SDCardClient sd(0);
        // SDCard::Info sd_info;
        // if(sd.GetInfo(sd_info) != ErrCodeE::Success) {
        //     emxloge("sdcard get info failed\n");
        //     break;
        // }
        // char path[64] = { 0 };
        // snprintf(path, sizeof(path) - 1
        //         , "%s/zn_debug"
        //         , sd_info.mountPoint);
        char path[64] = { 0 };
        snprintf(path, sizeof(path) - 1, "/mnt/sdcard/zn_debug");

        if (m_ageing_ctx->LoadZnDebugInfo(path) != ErrCodeE::Success) {
            emxloge("load configure file failed\n");
            break;
        }

        m_loop.Init("AgeingServer", [this]() {
            m_resp.DestroyMsgRpcRep();
        }, EuvLoop::Default);
        MsgRpcRep::Config cfg = {};
        cfg.loop = &m_loop;
        cfg.clientId = AgeingServerMsgId;
        cfg.buffer = m_buffer;
        cfg.size = EMX_TOOLS_BUFFER_SIZE;
        cfg.OnConnected = []() {};
        cfg.OnDisConnected = []() {};
        cfg.OnRecvReq = std::bind(&AgeingTool::OnRecvMsg, this, ph_1, ph_2, ph_3, ph_4, ph_5);
        m_resp.CreateMsgRpcRep(cfg);
        m_ageing_ctx->Start();
        m_loop.Start(false);
        ret = ErrCodeE::Success;
        emxlogd("AgeingServer Created\n");
    } while(false);

    return ret;
}

ErrCodeE AgeingTool::GetAgeingInfo(AgingInfo &info) {
    if (m_ageing_ctx == nullptr) {
        return ErrCodeE::Failure;
    }
    return m_ageing_ctx->LoadAgeingInfo(info);
}

ErrCodeE AgeingTool::OnRecvMsg(uint32_t module, uint32_t method,
                                 const char *data, int32_t size, std::string &out) {
    emxlogd("module[%u],method[%u],data#%s# size=%d\n"
                , module, method, size ? data : " ", size);
    switch ((EmxAgeingModule) method) {
        case EmxAgeingModule::GetInfo: {
            AgingInfo info;
            ErrCodeE e = GetAgeingInfo(info);
            out.assign((char *) &info, sizeof(info));
            return e;
        }
        default:
            emxloge("not support module[%u]\n", module);
            return ErrCodeE::OperationNotSupport;
    }
}
