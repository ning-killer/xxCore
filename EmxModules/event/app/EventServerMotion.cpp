/*
* @Author: xiong
* @Date: 2023/2/16
*/

#include "EventServerMotion.hpp"
#include "EventInner.hpp"

using namespace Emx;

std::vector<EventServerMotion *> EventServerMotion::m_array;

void EventServerMotion::Create(EventResource &res) {
    Json::Value json;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/eventCfg/motion.json", Misc::GetAppConfigsDir());
    if (!File::Exist(path)) {
        emxloge("%s not exist\n", path);
        return;
    }
    if (EasyJson::Load(path, json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path);
        return;
    }
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = new EventServerMotion(chn, res);
        m_array.push_back(e);
        res.pluginList.push_back(e);
        e->Start();
    }
}

void EventServerMotion::Destroy() {
    for (auto e: m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE EventServerMotion::OnRecvReq(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out) {
    int32_t chn;
    memcpy(&chn, data, sizeof(chn));
    if (chn >= (int32_t) m_array.size()) {
        emxloge("chn %d not exists\n", chn);
        return ErrCodeE::ResNotExist;
    }
    data += sizeof(chn);
    size -= sizeof(chn);
    switch ((EventMotionMethodE) method) {
        case EventMotionMethodE::MotionSet: {
            auto param = std::make_shared<Param>();
            param->Parse(data, size);
            return m_array[chn]->SetParam(param);
        }
        case EventMotionMethodE::MotionGet: {
            std::shared_ptr<Param> param;
            ErrCodeE e = m_array[chn]->GetParam(param);
            param->Serialize(out);
            return e;
        }
        default:
            emxloge("not support method[%u]\n", method);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE EventServerMotion::GetParam(std::shared_ptr<Param> &param) {
    param = m_param;
    return ErrCodeE::Success;
}

ErrCodeE EventServerMotion::SetParam(const std::shared_ptr<Param> &param) {
    m_param = param;
    return SaveParam();
}

ErrCodeE EventServerMotion::LoadParam() {
    try {
        ::Param paraFile("eventParam/motion");
        Json::Value root;
        if (paraFile.Get(root) != ErrCodeE::Success) {
            return ErrCodeE::Failure;
        }
        auto &json = root[m_chn];
        m_param->Parse(json);
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxloge("failed with %s\n", e.what());
        return ErrCodeE::Failure;
    }
}

ErrCodeE EventServerMotion::SaveParam() {
    try {
        ::Param paraFile("eventParam/motion");
        Json::Value root;
        if (paraFile.Get(root) != ErrCodeE::Success) {
            return ErrCodeE::Failure;
        }
        auto &json = root[m_chn];
        m_param->Serialize(json);
        return paraFile.Set(root);
    } catch (std::exception &e) {
        emxloge("failed with %s\n", e.what());
        return ErrCodeE::Failure;
    }
}

ErrCodeE EventServerMotion::Start() {
    if (LoadConfig() != ErrCodeE::Success) {
        emxloge("load config failed\n");
        return ErrCodeE::Failure;
    }
    InitializeCtx();
    if (LoadParam() != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }
    m_ai = new MediaClientAiDataAsync(m_res.loop);
    m_ai->Start(nullptr, std::bind(&EventServerMotion::ProcAiData, this, ph_1));
    emxlogd("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

void EventServerMotion::Stop() {
    EventEnd();
    m_ai->Stop();
    delete m_ai;
}

ErrCodeE EventServerMotion::LoadConfig() {
    Json::Value json;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/eventCfg/motion.json", Misc::GetAppConfigsDir());
    if (!File::Exist(path)) {
        emxloge("%s not exist\n", path);
        return ErrCodeE::ResNotExist;
    }
    if (EasyJson::Load(path, json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path);
        return ErrCodeE::Failure;
    }
    try {
        auto &map = json[m_chn]["sensitivityAreaMap"];
        for (int i = 0; i < (int) map.size(); i++)
            m_sensitivityAreaMap[i] = map[i].asInt();
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxloge("failed with %s\n", e.what());
        return ErrCodeE::Failure;
    }
}

void EventServerMotion::InitializeCtx() {
    m_delayProcessing = false;
    m_detectDelayTimer.Create(m_res.loop);
    //实时更新夜视状态
    m_night.Start(m_res.loop, m_res.buffer, sizeof(m_res.buffer),
                  [this](NightVision::Status &status) {
                      m_nightStatus = status;
                  });
}

void EventServerMotion::ProcAiData(MediaAi::AiInfoHeader &header) {
    //如果本事件没有使能则不处理数据，不处理数据会导致新的事件不会产生，已经产生的事件会随着各种定时器的超时而结束
    if (!m_param->ena)
        return;
    //如果配置了全局schedule,则在schedule不生效时间段不处理数据
    if (!m_res.scheduleActive)
        return;
    //仅监听如下类型数据
    if (header.type != MediaAi::AiTypeE::MotionDetection &&
        header.type != MediaAi::AiTypeE::PersonDetection)
        return;

    //根据通道号匹配,默认event的通道号和ai的通道号应该相同
    if (m_chn != header.chn)
        return;


    //获取AI数据
    if (m_ai->GetDataByInfo(header) != ErrCodeE::Success)
        return;

    auto *data = (MediaAi::AiInfoMotionDetectionData *) header.data;
    int maxArea = 0;
    for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoMotionDetectionData); i++) {
        if (EventInZone(header.srcPicSize, data[i], m_param->zone)) {
            if (data[i].Area() > maxArea)
                maxArea = data[i].Area();
        }
    }
    /*如果检测到运动面积大于90%，就不认为是移动物体*/
    if (maxArea > header.srcPicSize.w * header.srcPicSize.h * 90 / 100)
        return;

    if (m_param->sensitivity >= sizeof(m_sensitivityAreaMap)) {
        emxloge("sensitivity abnormal %d,should be[0-9],use default 5\n", m_param->sensitivity);
        m_param->sensitivity = 5;
    }

    int targetArea = m_sensitivityAreaMap[m_param->sensitivity];
    if (maxArea * 1000 / header.srcPicSize.w * header.srcPicSize.h > targetArea) {
        //事件产生
        if (!m_started) {
            //事件结束状态下的初次触发，先屏蔽一下等待延迟处理
            m_delayProcessing = true;
        }
        //如果是屏蔽状态
        if (m_delayProcessing) {
            //延迟1s观察情况，防止移动侦测提前触发
            m_detectDelayTimer.Start(1000, 0, [this]() {
                //如果夜视正在切换中，则不进行移动侦测
                if (m_nightStatus.switching) {
                    return;
                }
                //如果某些事件已经触发则不进行移动事件触发
                for (auto event : m_res.pluginList) {
                    if (event->IsStarted() &&
                        (event->GetType() == Event::TypeE::Person ||
                         event->GetType() == Event::TypeE::Tamper ||
                         event->GetType() == Event::TypeE::Invision)) {
                        return;
                    }
                }
                m_delayProcessing = false;
                EventTrigger();
            });
        } else {
            EventTrigger();
        }
    }
}
