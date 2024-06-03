#include "AlarmPassengerFlow.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmPassengerFlow::Create() {
    m_checkAlgStatusTimer.Create(m_ctx->loop);
    m_checkGatPostTimer.Create(m_ctx->loop);
    ManageGatPost();
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.passengerFlow.on) {
            return;
        }
        if (header.type != MediaAi::AiTypeE::PassengerFlowDetection) {
            return;
        }   
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success) {
            return;
        }
        if (!CheckHeadSrcSize(header.srcPicSize)) {
            emxloge("header.srcPicSize.h[%d];header.srcPicSize.w[%d]\n"
                    , header.srcPicSize.h, header.srcPicSize.w);
            return;
        }
        m_checkAlgStatusTimer.Stop();
        m_checkAlgStatusTimer.Start(2500, 0, [this]() {
            //note: 2s内未检测到目标输出，则认为目标离开,清空缓存
            emxlogd("data clear!\n");
            m_cacheTargets.clear();
        });
        auto *data = (MediaAi::AiInfoPassengerFlowDetectionData*) header.data;
        std::vector<MediaAi::AiInfoPassengerFlowDetectionData> corssTargets;
        emxlogd("obj size: %d\n", header.size / (int) sizeof(MediaAi::AiInfoPassengerFlowDetectionData));
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoPassengerFlowDetectionData); i++) {
            emxlogd("[%d]->id: %d\n", i, data[i].id);
            // 区域框检测
            if (AlarmInZone(header.srcPicSize, data[i].rect, m_ctx->env.passengerFlow.zone)) {
                // 拌线检测
                if (m_ctx->env.passengerFlow.line.IsCross(data[i].rect, header.srcPicSize)) {
                    corssTargets.emplace_back(data[i]);
                    emxlogd("current obejct have corss\n");
                } else {
                    emxlogd("obejct id: %d\n", data[i].id);
                    for (auto it = m_cacheTargets.begin(); it != m_cacheTargets.end(); ) {
                        if (it->id == data[i].id) {
                            // 删除重复id缓存
                            it = m_cacheTargets.erase(it);
                        } else {
                            ++it;
                        }
                    }
                    // 缓存未拌线目标
                    if ((int)m_cacheTargets.size() < m_maxCacheSize) {
                        m_cacheTargets.emplace_back(data[i]);
                    } else {
                        // 更新缓存
                        m_cacheTargets.erase(m_cacheTargets.begin());
                        m_cacheTargets.shrink_to_fit();
                        m_cacheTargets.emplace_back(data[i]);
                    }
                }  
            }
        }
        emxlogd("corssTargets size[%d],m_cacheTargets[%d]\n", corssTargets.size(), m_cacheTargets.size());
        if (corssTargets.size() == 0) {
            return;
        }
        EnvAlarmPassengerFlow::Direction direction = EnvAlarmPassengerFlow::BottomToTop;
        bool isShowOsd = false;
        for (auto corss : corssTargets) {
            for (auto iter = m_cacheTargets.begin(); iter != m_cacheTargets.end(); ) {
                emxlogd("corss id[%d],cache_id[%d]\n", corss.id, iter->id);
                if (corss.id == iter->id) {
                    // 判断越界方向
                    m_ctx->env.passengerFlow.line.CheckCrossDirection(direction, iter->rect, corss.rect, header.srcPicSize);
                    emxlogd("env statisticsMode[%d];cross style[%d]\n"
                            , m_ctx->env.passengerFlow.statisticsMode, direction);
                    m_cacheTargets.erase(iter);
                    isShowOsd = true;
                    if (m_ctx->env.passengerFlow.statisticsMode == EnvAlarmPassengerFlow::DirectionRule::BottomIn_TopOut) {
                        // 规则是下进上出, 下到上，则是离开; 上到下，则是进入
                        if (direction == EnvAlarmPassengerFlow::Direction::BottomToTop) {
                            m_outCount++;
                        } else if (direction == EnvAlarmPassengerFlow::Direction::TopToBottom) {
                            m_inCount++;
                        }
                    } else if (m_ctx->env.passengerFlow.statisticsMode == EnvAlarmPassengerFlow::DirectionRule::TopIn_BottomOut) {
                        // 规则是上进下出, 下到上，则是进入; 上到下，则是离开
                        if (direction == EnvAlarmPassengerFlow::Direction::BottomToTop) {
                            m_inCount++;
                        } else if (direction == EnvAlarmPassengerFlow::Direction::TopToBottom) {
                            m_outCount++;
                        }
                    }
                } else {
                    ++iter;
                }
            }
        }
        emxlogd("m_outCount: %d\n", m_outCount);
        emxlogd("m_inCount: %d\n", m_inCount);
        if (isShowOsd && m_ctx->env.passengerFlow.osdStatus) {
            ShowOsd();
        }
    });
    if (m_ctx->env.passengerFlow.osdStatus) {
        ShowOsd();
    }
    return ErrCodeE::Success;
}

void AlarmPassengerFlow::Destroy() {
    m_checkAlgStatusTimer.Destroy();
    m_checkGatPostTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

void AlarmPassengerFlow::ManageGatPost() {
    m_checkGatPostTimer.Start(0, 500, [this]() {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        auto local_tm = std::localtime(&now_time);
        if ((local_tm->tm_min == 30 && local_tm->tm_sec == 0)
            || (local_tm->tm_min == 0 && local_tm->tm_sec == 0)) {
            if (!m_ctx->env.passengerFlow.on) {
                return;
            }
            emxlogd("m_outCount: %d\n", m_outCount);
            emxlogd("m_inCount: %d\n", m_inCount);
            // xx::00::00 & xx::30::00 gat1400上报
            auto *param = new Gat1400Util::UploadTrafficData();
            PackUploadTrafficData(*param);
            Gat1400Client::Instance()->Upload(param);
            delete param;
            param = nullptr;
            if (local_tm->tm_hour == 24 && local_tm->tm_min == 0) {
                // 24:00::00 清空状态
                m_inCount = 0;
                m_outCount = 0;
            }
            if (m_ctx->env.passengerFlow.osdStatus) {
                ShowOsd();
            }
        }
    });
}

void AlarmPassengerFlow::PackUploadTrafficData(Gat1400Util::UploadTrafficData &param) {
    param.type = Gat1400Util::UploadType::Traffic;
    param.InfoKind = 1;
    param.LeftTopX = 0;
    param.LeftTopY = 0;
    param.RightBtmX = 0;
    param.RightBtmY = 0;
    param.deviceId = Gat1400Client::Instance()->GetDeviceId();
    param.LocationMarkTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.AppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.DisAppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.SourceID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
    param.Id = Gat1400Client::Instance()->GetFaceId(param.SourceID);
    param.trafficID = param.SourceID;
    param.endTime = param.DisAppearTime;
    param.inCount = m_inCount;
    param.outCount = m_outCount;
}

void AlarmPassengerFlow::ShowOsd() {

    m_ctx->env.passengerFlow.inCount = m_inCount;
    m_ctx->env.passengerFlow.outCount = m_outCount;
    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    osdParam.clear();
    if (m_ctx->env.offDuty.gatReportOn) {
        emxlogd("offDuty on\n");
        return;
    }
    if (!m_ctx->env.passengerFlow.on || !m_ctx->env.passengerFlow.osdStatus) {
        emxlogd("passengerFlow off\n");
        return;
    }
    if (osd.GetParam(osdParam) != ErrCodeE::Success) {
        emxloge("osd param get failed\n");
        return;
    }
    int fontsize = 0;
    int thickness = 0;
    MediaOSD::Margin margin;
    for (auto item : osdParam) {
        if (item->type == MediaOSD::TypeE::Time) {
            fontsize = item->time.size;
            thickness = item->time.thickness;
            margin = item->time.margin;
        }
    }

    auto paramIn = std::make_shared<MediaOSD::Param>();
    memset(paramIn.get(), 0, sizeof(MediaOSD::Param));
    paramIn->type = MediaOSD::TypeE::Text;
    paramIn->text.ena = true;
    memcpy(&paramIn->text.margin, &margin, sizeof(MediaOSD::Margin));
    paramIn->text.margin.rateY += fontsize + 5;
    paramIn->text.thickness = thickness;
    paramIn->text.size = fontsize;
    strncpy(paramIn->text.font, "font.ttf", sizeof(paramIn->text.font));
    snprintf(paramIn->text.content, sizeof(paramIn->text.content) - 1, "进入：%d", m_inCount);
    osdParam.push_back(paramIn);

    auto paramOut = std::make_shared<MediaOSD::Param>();
    memset(paramOut.get(), 0, sizeof(MediaOSD::Param));
    paramOut->type = MediaOSD::TypeE::Text;
    paramOut->text.ena = true;
    memcpy(&paramOut->text.margin, &margin, sizeof(MediaOSD::Margin));
    paramOut->text.margin.rateY = paramIn->text.margin.rateY + fontsize + 5;
    paramOut->text.thickness = thickness;
    paramOut->text.size = fontsize;
    strncpy(paramOut->text.font, "font.ttf", sizeof(paramOut->text.font));
    snprintf(paramOut->text.content, sizeof(paramOut->text.content) - 1, "离开：%d", m_outCount);
    osdParam.push_back(paramOut);

    osd.SetTempParam(osdParam);
}
