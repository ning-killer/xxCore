/*
* @Author: xiong
* @Date: 2023/2/16
*/

#include "EventServer.hpp"
#include "EventInner.hpp"
#include "EventServerMotion.hpp"

using namespace Emx;

void EventServer::Create(bool newThread) {
    if (ParseConfig() != ErrCodeE::Success) {
        return;
    }
    if (LoadParam() != ErrCodeE::Success) {
        return;
    }
    m_res.loop.Init("EventStdServer", [this]() {
        if (m_param->ena)
            DestroyAllPlugin();
        m_resp.DestroyMsgRpcRep();
        m_res.publish.DestroyMsgTopicPublisherAsync();
        m_schedule.Destroy();
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);


    //receive and process RPC request from net client
    MsgRpcRep::Config cfgRep = {};
    cfgRep.loop = &m_res.loop;
    cfgRep.clientId = EVENT_RESP_ID;
    cfgRep.buffer = m_res.buffer;
    cfgRep.size = sizeof(m_res.buffer);
    cfgRep.OnConnected = []() {emxlogd("Rpc connected\n");};
    cfgRep.OnDisConnected = []() { emxlogd("Rpc disconnected\n"); };
    cfgRep.OnRecvReq = std::bind(&EventServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfgRep);

    //publish link stat of net server
    MsgTopicPublisherAsync::Config cfgPub = {};
    cfgPub.loop = &m_res.loop;
    cfgPub.OnConnected = []() {emxlogd("Topic connected\n");};
    cfgPub.OnDisConnected = [this]() {
        emxlogd("Topic disconnected\n");
        m_res.publish.DestroyMsgTopicPublisherAsync();
    };
    m_res.publish.CreateMsgTopicPublisherAsync(cfgPub);
    //创建schedule，实时记录schedule的状态，通过scheduleActive传递给各个plugin
    m_schedule.Create(m_res.loop, "eventStd", m_param->schedule,
                      [this](const char *name, bool ena) { m_res.scheduleActive = ena; });
    CreateAllPlugin();
    m_res.loop.Start(newThread);
}

void EventServer::Destroy() {
    m_res.loop.StopAndDeInit();
}

void EventServer::CreateAllPlugin() {
    EventServerMotion::Create(m_res);
    std::map<Event::TypeE, int> typeCount;
    for (auto &plugs:m_res.pluginList) {
        typeCount[plugs->GetType()]++;
    }
    m_events.clear();
    for (auto &type:typeCount) {
        Event::Channel chn = {};
        chn.type = type.first;
        chn.chnNum = type.second;
        m_events.emplace_back(chn);
    }
}

void EventServer::DestroyAllPlugin() {
    EventServerMotion::Destroy();
    m_res.pluginList.clear();
}

ErrCodeE EventServer::ParseConfig() {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/eventCfg/global.json", Misc::GetAppConfigsDir());
    if (!File::Exist(path)) {
        emxloge("%s not exist\n", path);
        return ErrCodeE::ResNotExist;
    }
    Json::Value json;
    if (EasyJson::Load(path, json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path);
        return ErrCodeE::Failure;
    }
    auto &timeMs = json["timeMs"];
    m_res.eventEndJudgeTimeMs = timeMs["endJudge"].asInt();
    m_res.eventContinueTimeMinMs = timeMs["continueMin"].asInt();
    m_res.eventContinueTimeMaxMs = timeMs["continueMax"].asInt();
    return ErrCodeE::Success;
}

ErrCodeE EventServer::OnRecvReq(uint32_t module, uint32_t method, const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data#%s# size=%d\n", module, method, size ? data : " ", size);
    switch ((EventModuleE) module) {
        case EventModuleE::Global:
            switch ((EventGlobalMethodE) method) {
                case EventGlobalMethodE::ListGet: {
                    std::vector<Event::Channel> channels;
                    auto e = GetSupportedEventChannel(channels);
                    if (e != ErrCodeE::Success) {
                        return e;
                    }
                    int num = (int) channels.size();
                    out.append((char *) &num, sizeof(num));
                    for (auto &c:channels) {
                        out.append((char *) &c, sizeof(c));
                    }
                    return e;
                }
                case EventGlobalMethodE::GlobalSet: {
                    auto param = std::make_shared<Param>();
                    param->Parse(data, size);
                    return SetParam(param);
                }
                case EventGlobalMethodE::GlobalGet: {
                    auto param = std::make_shared<Param>();
                    auto e = GetParam(param);
                    if (e != ErrCodeE::Success) {
                        return e;
                    }
                    param->Serialize(out);
                    return e;
                }
                default:
                    emxloge("not support method[%u]\n", module);
                    return ErrCodeE::OperationNotSupport;
            }
        case EventModuleE::Motion:
            return EventServerMotion::OnRecvReq(module, method, data, size, out);
        default:
            emxloge("not support module[%u]\n", module);
            return ErrCodeE::OperationNotSupport;
    }

}

ErrCodeE EventServer::GetSupportedEventChannel(std::vector<Event::Channel> &channels) {
    channels = m_events;
    return ErrCodeE::Success;
}


ErrCodeE EventServer::GetParam(std::shared_ptr<Param> &param) {
    param = m_param;
    return ErrCodeE::Success;
}

ErrCodeE EventServer::SetParam(const std::shared_ptr<Param> &param) {
    DestroyAllPlugin();
    m_param = param;
    if (m_param->ena) {
        CreateAllPlugin();
        m_schedule.Update(m_param->schedule);
    }
    SaveParam();
    return ErrCodeE::Success;
}

ErrCodeE EventServer::LoadParam() {
    try {
        ::Param paraFile("eventParam/global");
        Json::Value json;
        paraFile.Get(json);
        m_param = std::make_shared<Param>();
        m_param->Parse(json);
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxloge("failed with %s\n", e.what());
        return ErrCodeE::Failure;
    }
}

ErrCodeE EventServer::SaveParam() {
    try {
        Json::Value json;
        m_param->Serialize(json);
        ::Param paraFile("eventParam/global");
        return paraFile.Set(json);
    } catch (std::exception &e) {
        emxloge("failed with %s\n", e.what());
        return ErrCodeE::Failure;
    }
}

