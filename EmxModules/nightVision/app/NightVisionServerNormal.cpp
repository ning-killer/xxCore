/*
* @Author: xiong
* @Date: 2022/11/30
*/

#include <NightVisionInner.hpp>
#include "NightVisionServerNormal.hpp"

using namespace Emx;

ErrCodeE NightVisionServerNormal::CreateSub(Json::Value &json) {
    try {
        auto &normal = json["normal"];
        m_cfg.ispChn = normal["ispChn"].asInt();
        m_cfg.adcChn = normal["adcChn"].asInt();

        for (auto &item : normal["channel"]) {
            Config::Channel chn = {};
            chn.isp = item["isp"].asInt();
            chn.irCut = item["irCut"].asInt();
            chn.whiteLight = item["whiteLight"].asInt();
            chn.irLight = item["irLight"].asInt();
            m_cfg.chn.push_back(chn);
        }

        auto &day2ir = normal["day2ir"];
        m_cfg.day2irUsingHard = day2ir["usingHard"].asBool();
        for (int i = 0; i < Config::SenMaxNum; i++) {
            m_cfg.day2ir[i].hard = day2ir["hard"][i].asInt();
            m_cfg.day2ir[i].soft.iso = day2ir["soft"]["iso"][i].asInt();
        }
        auto &ir2day = normal["ir2day"];
        m_cfg.ir2dayUsingHard = ir2day["usingHard"].asBool();
        for (int i = 0; i < Config::SenMaxNum; i++) {
            m_cfg.ir2day[i].hard = ir2day["hard"][i].asInt();
            m_cfg.ir2day[i].soft.iso = ir2day["soft"]["iso"][i].asInt();
            m_cfg.ir2day[i].soft.redGain = ir2day["soft"]["redGain"][i].asInt();
            m_cfg.ir2day[i].soft.blueGain = ir2day["soft"]["blueGain"][i].asInt();
        }

        auto &day2color = normal["day2color"];
        m_cfg.day2colorUsingHard = day2color["usingHard"].asBool();
        for (int i = 0; i < Config::SenMaxNum; i++) {
            m_cfg.day2color[i].hard = day2color["hard"][i].asInt();
            m_cfg.day2color[i].soft.iso = day2color["soft"]["iso"][i].asInt();
        }

        auto &color2day = normal["color2day"];
        m_cfg.color2dayUsingHard = color2day["usingHard"].asBool();
        for (int i = 0; i < Config::SenMaxNum; i++) {
            m_cfg.color2day[i].hard = color2day["hard"][i].asInt();
            m_cfg.color2day[i].soft.isoMin = color2day["soft"]["isoMin"][i].asInt();
            m_cfg.color2day[i].soft.isoMax = color2day["soft"]["isoMax"][i].asInt();
        }
        if (json["2dayDelay"].isInt()) {
            m_todayDelay = json["2dayDelay"].asInt();
        }
        if (json["2irDelay"].isInt()) {
            m_toirDelay = json["2irDelay"].asInt();
        }

        m_isp.Start(m_loop, m_buffer, sizeof(m_buffer), [this](MediaISP::Info &info) {
            //匹配使用的isp通道
            if (info.chn == m_cfg.ispChn) {
                m_ispInfo = info;
                m_ispInfo.iso = info.iso;
            }
        });

        m_timer.Create(m_loop);
        m_timer.Start(3000, 300, std::bind(&NightVisionServerNormal::OnAutoModeJudge, this));

        return ErrCodeE::Success;
    } catch (std::exception &e) {
        return ErrCodeE::Failure;
    }
}

void NightVisionServerNormal::DestroySub() {
    m_isp.Stop();
    m_timer.Destroy();
}

void NightVisionServerNormal::DoSwitch(NightVision::ModeE mode) {
    switch (mode) {
        case ModeE::Day: {
            for (auto &chn :m_cfg.chn) {
                m_bsp.irCut->SwitchToNight(false, chn.irCut);
                m_bsp.irLight->SetEna(false, chn.irLight);
                m_bsp.whiteLight->SetEna(false, chn.whiteLight);
                MediaClientISP isp(chn.isp);
                emxlogd("todayDelay[%d]\n", m_todayDelay);
                usleep(m_todayDelay * 1000);
                isp.SetRunMode(MediaISP::RunModeE::Day);
            }
            break;
        }
        case ModeE::IrNightVision: {
            for (auto &chn :m_cfg.chn) {
                MediaClientISP isp(chn.isp);
                isp.SetRunMode(MediaISP::RunModeE::Night);
                m_bsp.whiteLight->SetEna(false, chn.whiteLight);
                m_bsp.irLight->SetEna(true, chn.irLight);
                emxlogd("toirDelay[%d]\n", m_toirDelay);
                usleep(m_toirDelay * 1000);
                m_bsp.irCut->SwitchToNight(true, chn.irCut);
            }
            break;
        }
        case ModeE::ColorNightVision: {
            for (auto &chn :m_cfg.chn) {
                m_bsp.irCut->SwitchToNight(false, chn.irCut);
                m_bsp.irLight->SetEna(false, chn.irLight);
                m_bsp.whiteLight->SetLuma(Bsp::WhiteLight::LumaMaxValue, chn.whiteLight);
                m_bsp.whiteLight->SetEna(true, chn.whiteLight);
                MediaClientISP isp(chn.isp);
                isp.SetRunMode(MediaISP::RunModeE::NightWithLight);
            }
            break;
        }
        default:
            emxloge("mode %d not support\n", (int) mode);
            return;
    }
}

void NightVisionServerNormal::OnAutoModeJudge() {
    if (m_running.manual) {
        m_debounceCount = 0;
        return;
    }
    switch (m_status.mode) {
        case ModeE::Day:
            if (m_running.autoMode == AutoModeE::IrNightVision)
                Day2IrJudge(); // 优先硬其次软
            else
                Day2ColorJudge(); // 可选硬、软，建议纯软
            break;
        case ModeE::IrNightVision:
            if (m_running.autoMode == AutoModeE::IrNightVision) {
                Ir2DayJudge(); // 优先硬其次软
            } else {
                //黑白切换全彩模式, 先灭灯
                Switch(NightVision::ModeE::Day);
                m_debounceCount = 0;
            }
            break;
        case ModeE::ColorNightVision: {
            if (m_running.autoMode == AutoModeE::ColorNightVision) {
                Color2DayJudge(); // 优先软，其次硬
            } else {
                //全彩切换黑白模式, 先灭灯
                Switch(NightVision::ModeE::Day);
                m_debounceCount = 0; 
            }
            
            break;
        }
        default:
            break;
    }
}

void NightVisionServerNormal::Day2IrJudge() {
    // day -> IR night
    if (m_cfg.day2irUsingHard) { // 使用硬件光敏
        auto &hard = m_cfg.day2ir[m_param.sensitivity.day2ir].hard;
        m_debounceCount = m_bsp.adc->GetLumaValue(m_cfg.adcChn) < hard ? m_debounceCount + 1 : 0;
#if 0
        emxlogw("count: %d; hard->[%d : %d]\n", m_debounceCount, hard, m_bsp.adc->GetLumaValue(m_cfg.adcChn));
#endif
    } else { // 使用软光敏
        auto &soft = m_cfg.day2ir[m_param.sensitivity.day2ir].soft;
        m_debounceCount = m_ispInfo.iso > soft.iso ? m_debounceCount + 1 : 0;
    }
    if (m_debounceCount >= 10) {
        Switch(NightVision::ModeE::IrNightVision);
        m_debounceCount = 0;
    }
}

void NightVisionServerNormal::Ir2DayJudge() {
    // IR night -> day
    if (m_cfg.ir2dayUsingHard) { // 使用硬件光敏
        auto &hard = m_cfg.ir2day[m_param.sensitivity.ir2day].hard;
        m_debounceCount = m_bsp.adc->GetLumaValue(m_cfg.adcChn) > hard ? m_debounceCount + 1 : 0;
#if 0
        emxlogw("count: %d; hard->[%d : %d]\n", m_debounceCount, hard, m_bsp.adc->GetLumaValue(m_cfg.adcChn));
#endif
    } else { // 使用软光敏
        auto &soft = m_cfg.ir2day[m_param.sensitivity.ir2day].soft;
        if (m_ispInfo.blueGain > soft.blueGain &&
            m_ispInfo.redGain > soft.redGain &&
            m_ispInfo.iso < soft.iso) {
            m_debounceCount++;
        } else {
            m_debounceCount = 0;
        }
    }
    if (m_debounceCount >= 10) {
        Switch(NightVision::ModeE::Day);
        m_debounceCount = 0;
    }
}

void NightVisionServerNormal::Day2ColorJudge() {
    // day -> color night
    if (m_cfg.day2colorUsingHard) { // 使用硬件光敏
        auto &hard = m_cfg.day2color[m_param.sensitivity.day2color].hard;
        m_debounceCount = m_bsp.adc->GetLumaValue(m_cfg.adcChn) < hard ? m_debounceCount + 1 : 0;
    } else { // 使用软光敏
        auto &soft = m_cfg.day2color[m_param.sensitivity.day2color].soft;
        m_debounceCount = m_ispInfo.iso > soft.iso ? m_debounceCount + 1 : 0;
#if 0
        emxlogw("count: %d; soft->[%d : %d]\n", m_debounceCount, soft.iso, m_ispInfo.iso);
#endif
    }
    if (m_debounceCount >= 10) {
        Switch(NightVision::ModeE::ColorNightVision);
        m_debounceCount = 0;
    }
}

void NightVisionServerNormal::Color2DayJudge() {
    // color night -> day
    if (m_cfg.color2dayUsingHard) { // 使用硬件光敏
        auto &hard = m_cfg.color2day[m_param.sensitivity.color2day].hard;
        m_debounceCount = m_bsp.adc->GetLumaValue(m_cfg.adcChn) > hard ? m_debounceCount + 1 : 0;
    } else { // 使用软光敏
        auto &soft = m_cfg.color2day[m_param.sensitivity.color2day].soft;
        int luma = m_bsp.whiteLight->GetLuma();
#if 0
        emxlogw("luma: %d; Min(%d),Max(%d)\n", luma, Bsp::WhiteLight::LumaMinValue, Bsp::WhiteLight::LumaMaxValue);
#endif
        if (m_ispInfo.iso < soft.isoMin) {
            // 环境有些亮，则尝试降低白光亮度
            if (luma > Bsp::WhiteLight::LumaMinValue) {
                luma -= (Bsp::WhiteLight::LumaMaxValue - Bsp::WhiteLight::LumaMinValue) / 20;
                luma = luma < Bsp::WhiteLight::LumaMinValue ? Bsp::WhiteLight::LumaMinValue : luma;
                // 防止亮度变化的时候移动告警，这里发布切换状态
                PublishSwitching();
                for (auto &chn :m_cfg.chn) {
                    m_bsp.whiteLight->SetLuma(luma, chn.whiteLight);
                }
                // 白光变化后本次ISO不具有参考价值，重置m_debounceCount
                m_debounceCount = 0;
            }
        } else if (m_ispInfo.iso > soft.isoMax) {
            // 环境有些暗，则尝试提高白光亮度
            if (luma < Bsp::WhiteLight::LumaMaxValue) {
                luma += (Bsp::WhiteLight::LumaMaxValue - Bsp::WhiteLight::LumaMinValue) / 20;
                luma = luma > Bsp::WhiteLight::LumaMaxValue ? Bsp::WhiteLight::LumaMaxValue : luma;
                // 防止亮度变化的时候移动告警，这里发布切换状态
                PublishSwitching();
                for (auto &chn :m_cfg.chn) {
                    m_bsp.whiteLight->SetLuma(luma, chn.whiteLight);
                }
                // 白光变化后本次ISO不具有参考价值，重置m_debounceCount
                m_debounceCount = 0;
            }
        }
        // 如果当前白光灯已经是最暗，并且ISO还是小于最低阈值，则延时切换到白天
        if (m_ispInfo.iso < soft.isoMin && luma == Bsp::WhiteLight::LumaMinValue) {
            m_debounceCount++;
        } else {
            m_debounceCount = 0;
        }
#if 0
        emxlogw("count: %d; soft->[min(%d),max(%d) : %d]\n", m_debounceCount, soft.isoMin, soft.isoMax, m_ispInfo.iso);
#endif
    }
    if (m_debounceCount >= 10) {
        Switch(NightVision::ModeE::Day);
        m_debounceCount = 0;
    }
}

