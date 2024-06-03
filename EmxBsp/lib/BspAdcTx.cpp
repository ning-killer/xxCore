//
// Created by xiong on 2022/6/30.
//

#include "BspAdcTx.hpp"
#include "BspUtils.hpp"

using namespace Emx;

BspAdcTx::BspAdcTx() : Bsp::Adc() {
    m_created = false;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    do {
        if (!inst->GetCfg().isMember("adcCvitek"))
            break;
        auto &e = inst->GetCfg()["adcCvitek"];
        m_defaultChnIdx = e["defaultChnIdx"].asInt();
        for (auto &item : e["channels"]) {
            Channel chn = {};
            chn.chnNum = item["chnNum"].asInt();
            chn.reference = item["reference"].asInt();
            chn.adcValueAtMaxLuma = item["adcValueAtMaxLuma"].asInt();
            chn.adcValueAtMinLuma = item["adcValueAtMinLuma"].asInt();
            m_channels.emplace_back(chn);
        }
        m_created = true;
    } while (false);
    inst->UnLock();
}

BspAdcTx::~BspAdcTx() {
    m_channels.clear();
}

int BspAdcTx::GetLumaValue(int chn) {
    if (!m_created)
        return -1;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return -1;
    int val = GetAdcValue(chn);
    if (val == -1)
        return val;
    int min, max;

    auto &channel = m_channels[chn];
    if (channel.adcValueAtMinLuma > channel.adcValueAtMaxLuma) {
        val = channel.adcValueAtMinLuma - val;
        min = channel.adcValueAtMaxLuma;
        max = channel.adcValueAtMinLuma;
    } else {
        min = channel.adcValueAtMinLuma;
        max = channel.adcValueAtMaxLuma;
    }
    return val * 1000 / (max - min);
}

int BspAdcTx::GetAdcValue(int chn) {
    int val = -1;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    do {
        if (!m_created)
            break;
        if (chn < 0) chn = m_defaultChnIdx;
        if (chn >= (int) m_channels.size())
            break;
        char path[EMX_MAX_PATH_SIZE] = {};
        snprintf(path, sizeof(path),
                 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
        FILE *fp = fopen(path, "r+");
        if (fp) {
            fprintf(fp, "%d", m_channels[chn].reference);
            fclose(fp);
            fp = fopen(path, "r");
            if (fp) {
                char buf[16] = {};
                if (fread(buf, sizeof(char), sizeof(buf) - 1, fp) > 0) {
                    val = strtol(buf, nullptr, 10);
                }
                fclose(fp);
            }
        }
    } while (false);
    inst->UnLock();
    return val;
}