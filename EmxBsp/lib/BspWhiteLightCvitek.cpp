//
// Created by xiong on 2022/6/29.
//

#include "BspWhiteLightCvitek.hpp"
#include "BspUtils.hpp"

using namespace Emx;

BspWhiteLightCvitek::BspWhiteLightCvitek() : Bsp::WhiteLight() {
    m_created = false;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    do {
        if (!inst->GetCfg().isMember("whiteLightCvitek"))
            break;
        auto &cfg = inst->GetCfg()["whiteLightCvitek"];
        m_defaultChnIdx = cfg["defaultChnIdx"].asInt();
        for (auto &item : cfg["channels"]) {
            Channel chn = {};
            chn.luma = 0;
            chn.chipNum = item["chipNum"].asInt();
            chn.pwmNum = item["pwmNum"].asInt();
            chn.period = item["period"].asInt();
            chn.dutyMin = item["dutyMin"].asInt();
            chn.dutyMax = item["dutyMax"].asInt();

            char path[EMX_MAX_PATH_SIZE];
            sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d", chn.chipNum, chn.pwmNum);
            if (!File::Exist(path)) {
                if (item.isMember("system")) {
                    Cmd::RunCheck(item["system"].asCString());
                }
                //注册pwm
                sprintf(path, "/sys/class/pwm/pwmchip%d/export", chn.chipNum);
                FILE *fp = fopen(path, "w");
                if (!fp) {
                    emxloge("cannot open %s\n", path);
                    break;
                }
                fprintf(fp, "%d", chn.pwmNum);
                fclose(fp);
                /*设置 PWM 一个周期的持续时间， 单位为 ns*/
                sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/period", chn.chipNum, chn.pwmNum);
                fp = fopen(path, "w");
                if (!fp) {
                    emxloge("cannot open %s\n", path);
                    break;
                }
                fprintf(fp, "%d", chn.period);
                fclose(fp);
                /*设置一个周期中的”ON” 时间，单位为 ns*/
                sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chn.chipNum, chn.pwmNum);
                fp = fopen(path, "w");
                if (!fp) {
                    emxloge("cannot open %s\n", path);
                    break;
                }
                fprintf(fp, "%d", 0);
                fclose(fp);
                /*设置 PWM 使能*/
                sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chn.chipNum, chn.pwmNum);
                fp = fopen(path, "w");
                if (!fp) {
                    emxloge("cannot open %s\n", path);
                    break;
                }
                fprintf(fp, "%d", 1);
                fclose(fp);
            }
            m_channels.emplace_back(chn);
        }
        m_created = true;
    } while (false);
    inst->UnLock();
}

BspWhiteLightCvitek::~BspWhiteLightCvitek() {
    m_channels.clear();
}

ErrCodeE BspWhiteLightCvitek::SetEna(bool ena, int chn) {
    if (!m_created)
        return ErrCodeE::ResNotAvailable;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return ErrCodeE::IllegalParam;
    ErrCodeE e;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    if (ena) {
        e = DoSetLuma(m_channels[chn].luma, chn);
    } else {
        e = DoSetLuma(-1, chn);
    }
    inst->UnLock();
    return e;
}

bool BspWhiteLightCvitek::GetEna(int chn) {
    if (!m_created)
        return false;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return false;
    auto &channel = m_channels[chn];
    auto inst = BspUtils::GetInst();
    inst->Lock();
    bool ena = false;
    do {
        char path[EMX_MAX_PATH_SIZE];
        sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", channel.chipNum, channel.pwmNum);
        FILE *fp = fopen(path, "r");
        if (!fp) {
            emxloge("cannot open %s\n", path);
            break;
        }
        char buf[16] = {};
        fread(buf, sizeof(char), sizeof(buf) - 1, fp);
        fclose(fp);
        ena = strtol(buf, nullptr, 10) != 0;
    } while (false);
    inst->UnLock();
    return ena;
}

ErrCodeE BspWhiteLightCvitek::SetLuma(int luma, int chn) {
    if (!m_created)
        return ErrCodeE::ResNotAvailable;
    if (luma > LumaMaxValue || luma < LumaMinValue) {
        emxloge("illegal luma value %d\n", luma);
        return ErrCodeE::IllegalParam;
    }
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return ErrCodeE::IllegalParam;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    ErrCodeE ret = DoSetLuma(luma, chn);
    if (ret == ErrCodeE::Success)
        m_channels[chn].luma = luma;
    inst->UnLock();
    return ret;
}

int BspWhiteLightCvitek::GetLuma(int chn) {
    if (!m_created)
        return -1;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return -1;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    int luma = m_channels[chn].luma;
    inst->UnLock();
    return luma;
}

ErrCodeE BspWhiteLightCvitek::DoSetLuma(int luma, int chn) {
    auto &channel = m_channels[chn];
    char path[EMX_MAX_PATH_SIZE];
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", channel.chipNum, channel.pwmNum);
    FILE *fp = fopen(path, "w");
    if (!fp) {
        emxloge("cannot open %s\n", path);
        return ErrCodeE::OpenFailed;
    }
    if (luma == -1)
        fprintf(fp, "%d", 0);
    else
        fprintf(fp, "%d", (channel.dutyMax - channel.dutyMin) * luma / (LumaMaxValue - LumaMinValue) + channel.dutyMin);
    fclose(fp);
    return ErrCodeE::Success;
}