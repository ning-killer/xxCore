/*
* @Author: xiong
* @Date: 2022/11/30
*/

#include "NightVisionServerBase.hpp"

#include <utility>
#include "NightVisionInner.hpp"

using namespace Emx;

ErrCodeE NightVisionServerBase::Create(OnCreated cb, bool newThread) {
    m_cb = std::move(cb);
    auto path = new std::string(Misc::GetAppConfigsDir());
    path->append("/nightVisionCfg.json");
    if (!File::Exist(path->c_str())) {
        printf("%s not exist\n", path->c_str());
        delete path;
        return ErrCodeE::Failure;
    }
    Json::Value json;
    if (EasyJson::Load(path->c_str(), json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path->c_str());
        delete path;
        return ErrCodeE::Failure;
    }
    delete path;
    postSwitchTimeMs = json["postSwitchTimeMs"].asInt();
    if (LoadParam() != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }

    m_running = m_param;

    m_loop.Init("NightVisionServer", [this]() {
        m_pub.DestroyMsgTopicPublisherAsync();
        m_switchTimer.Destroy();
        m_resp.DestroyMsgRpcRep();
        DestroySub();
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    //receive and process RPC request from client

    m_respConnected = false;
    MsgRpcRep::Config cfgRep = {};
    cfgRep.loop = &m_loop;
    cfgRep.clientId = NIGHT_VISION_RESP_ID;
    cfgRep.buffer = m_buffer;
    cfgRep.size = sizeof(m_buffer);
    cfgRep.OnConnected = [this]() {
        m_respConnected = true;
        if (m_pubConnected && m_cb) {
            m_cb();
        }
    };
    cfgRep.OnDisConnected = []() {};
    cfgRep.OnRecvReq = std::bind(&NightVisionServerBase::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfgRep);

    m_pubConnected = false;
    MsgTopicPublisherAsync::Config cfgPub = {};
    cfgPub.loop = &m_loop;
    cfgPub.OnConnected = [this]() {
        m_pubConnected = true;
        if (m_respConnected && m_cb) {
            m_cb();
        }
    };
    m_pub.CreateMsgTopicPublisherAsync(cfgPub);

    m_switchTimer.Create(m_loop);
    m_status.switching = false;
    m_status.mode = NightVision::ModeE::Day;

    if (CreateSub(json) != ErrCodeE::Success) {
        emxloge("create sub failed\n");
        return ErrCodeE::Failure;
    }
    json.clear();
    emxlogd("NightVisionServer newThread(%d)\n",newThread);
    m_loop.Start(newThread);
    return ErrCodeE::Success;
}

ErrCodeE NightVisionServerBase::LoadParam() {
    try {
        ::Param param("nightVision");
        Json::Value json;
        ErrCodeE e = param.Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get failed\n");
            return e;
        }
        m_param.manual = json["manual"].asBool();
        m_param.autoMode = (AutoModeE) json["autoMode"].asInt();
        m_param.manualMode = (ModeE) json["manualMode"].asInt();
        m_param.sensitivity.day2ir = json["sensitivity"]["day2ir"].asInt();
        m_param.sensitivity.ir2day = json["sensitivity"]["ir2day"].asInt();
        m_param.sensitivity.day2color = json["sensitivity"]["day2color"].asInt();
        m_param.sensitivity.color2day = json["sensitivity"]["color2day"].asInt();
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE NightVisionServerBase::SaveParam() {
    try {
        ::Param param("nightVision");
        Json::Value json;
        json["manual"] = m_param.manual;
        json["autoMode"] = (int) m_param.autoMode;
        json["manualMode"] = (int) m_param.manualMode;
        auto &sensitivity = json["sensitivity"];
        sensitivity["day2ir"] = m_param.sensitivity.day2ir;
        sensitivity["ir2day"] = m_param.sensitivity.ir2day;
        sensitivity["day2color"] = m_param.sensitivity.day2color;
        sensitivity["color2day"] = m_param.sensitivity.color2day;
        ErrCodeE e = param.Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE NightVisionServerBase::OnRecvReq(uint32_t module, uint32_t method,
                                          const char *data, int32_t size, std::string &out) {
    switch ((NightVisionMethodE) method) {
        case NightVisionMethodE::GetParam: {
            NightVision::Param param = {};
            auto e = GetParam(param);
            out.append((char *) &param, sizeof(param));
            return e;
        }
        case NightVisionMethodE::SetParam: {
            return SetParam(*(NightVision::Param *) data);
        }
        case NightVisionMethodE::GetParamTemp: {
            NightVision::Param param = {};
            auto e = GetParamTemp(param);
            out.append((char *) &param, sizeof(param));
            return e;
        }
        case NightVisionMethodE::SetParamTemp: {
            return SetParamTemp(*(NightVision::Param *) data);
        }
        case NightVisionMethodE::GetStatus: {
            Status status = {};
            auto e = GetStatus(status);
            out.append((char *) &status, sizeof(status));
            return e;
        }
        default:
            emxloge("method %d not support\n", (int) method);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE NightVisionServerBase::GetParam(NightVision::Param &param) {
    param = m_param;
    return ErrCodeE::Success;
}

ErrCodeE NightVisionServerBase::SetParam(const NightVision::Param &param) {
    m_param = param;
    SetParamTemp(param);
    return SaveParam();
}

ErrCodeE NightVisionServerBase::GetParamTemp(NightVision::Param &param) {
    param = m_running;
    return ErrCodeE::Success;
}

ErrCodeE NightVisionServerBase::SetParamTemp(const NightVision::Param &param) {
    m_running = param;
    if (param.manual)
        Switch(param.manualMode);
    return ErrCodeE::Success;
}

ErrCodeE NightVisionServerBase::GetStatus(Status &status) {
    status = m_status;
    return ErrCodeE::Success;
}

void NightVisionServerBase::Switch(NightVision::ModeE mode) {
    //通知其他进程夜视状态正要切换，用于使用类似移动侦测的程序屏蔽检测
    m_status.mode = mode;
    PublishSwitching();
    emxlogi("switch to %s\n",
            mode == NightVision::ModeE::Day ? "day" :
            mode == NightVision::ModeE::IrNightVision ? "IR Night" : "color night");
    DoSwitch(mode);
}

void NightVisionServerBase::PublishSwitching() {
    m_status.switching = true;
    m_pub.Publish(NightVisionTopicId, (char *) &m_status, sizeof(m_status));
    //切换结束后需要延迟等待PQ收敛
    m_switchTimer.Start(postSwitchTimeMs, 0, [this]() {
        //收敛完成，通知其他模块切换结束
        m_status.switching = false;
        m_pub.Publish(NightVisionTopicId, (char *) &m_status, sizeof(m_status));
    });
}

