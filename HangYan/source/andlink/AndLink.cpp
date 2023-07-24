//
// Created by xiong on 2022/6/23.
//

#include "AndLink.hpp"
#include "NetClient.hpp"
#include "andlink_adapt.h"
#include "OvdUtils.hpp"
#include <algorithm>

using namespace Emx;

void AndLink::Create(OvdCtx *ctx) {
    if (m_created)
        return;
    andlink_attr_t attr = {};
    Net::DevE dev = Net::DevE::Wifi;
    NetMainDev::Get(dev);
    attr.cfgNetMode = dev == Net::DevE::Eth ? NETWOKR_MODE_WIRED : NETWOKR_MODE_WIFI;

    strncpy(attr.deviceVendor, ctx->env.manufacturers.deviceVendor, sizeof(attr.deviceVendor));//厂商名称英文或中文拼音拼写，尽量简短
    strncpy(attr.deviceBrand, ctx->env.manufacturers.deviceBrand, sizeof(attr.deviceBrand));//设备品牌
    strncpy(attr.deviceModel, ctx->env.devInfo.GetOuter()->type, sizeof(attr.deviceModel));//设备型号
    strncpy(attr.deviceType, ctx->env.manufacturers.deviceType, sizeof(attr.deviceType));//设备类型ID,即门户上的 产品ID
    strncpy(attr.deviceMac, ctx->env.mac.Get(), sizeof(attr.deviceMac));// 厂商可以填mac或sn,即设备唯一标识
    strncpy(attr.andlinkToken, ctx->env.burn.andlinkID.data(),
            sizeof(attr.andlinkToken));//设备在开发者门户注册的产品类型对应的平台Token
    strncpy(attr.productToken, ctx->env.burn.andlinkKey.data(),
            sizeof(attr.productToken));//设备在开发者门户注册的产品类型Token，平台会检查其合法性，非法则不允许注册
    strncpy(attr.firmWareVersion, ctx->env.devInfo.GetOuter()->version,
            sizeof(attr.firmWareVersion));// 厂商可以填mac或sn,即设备唯一标识
    strncpy(attr.softWareVersion, ctx->env.devInfo.GetOuter()->version,
            sizeof(attr.softWareVersion));// 厂商可以填mac或sn,即设备唯一标识
    strncpy(attr.cfgPath, ctx->deviceJsonCfg["andlinkPath"].asCString(),
            sizeof(attr.cfgPath));//供sdk存储配置文件的系统路径，此路径需可读可写，断电数据不丢失
    strncpy(attr.dm_info.cmei, ctx->env.burn.cmei.data(), sizeof(attr.dm_info.cmei));// 设备唯一标识，必选
    attr.dm_info.authMode = 1;// 0表示类型认证，1表示设备认证，设备认证时，需使用authId和authKey
    strncpy(attr.dm_info.authId, ctx->env.burn.andlinkID.data(), sizeof(attr.dm_info.authId));// 用于生成工作密钥，设备认证必选
    strncpy(attr.dm_info.authKey, ctx->env.burn.andlinkKey.data(), sizeof(attr.dm_info.authKey));// 用于生成工作密钥，设备认证必选
    std::string s_mac(ctx->env.mac.Get(), strlen(ctx->env.mac.Get()));
    s_mac.erase(std::remove(s_mac.begin(), s_mac.end(), ':'), s_mac.end());
    strncpy(attr.dm_info.mac, s_mac.c_str(), sizeof(attr.dm_info.mac));// 设备真实MAC，全大写不带冒号
    strncpy(attr.dm_info.sn, ctx->env.sn.Get(), sizeof(attr.dm_info.sn));// 设备SN，必选
    strncpy(attr.dm_info.reserve, ctx->env.manufacturers.reserve, sizeof(attr.dm_info.reserve));// 标记字段，可选
    char tmp[32];
    strncpy(attr.dm_info.manuDate, OvdUtils::GetDateString(tmp), sizeof(attr.dm_info.manuDate));// 设备生产日期，格式为年-月
    strncpy(attr.dm_info.OS, ctx->env.manufacturers.os, sizeof(attr.dm_info.OS));// 操作系统
    strncpy(attr.dm_info.chips_type, "Main", sizeof(attr.dm_info.chips_type));//  芯片类型，如Main/WiFi/Zigbee/BLE等
    strncpy(attr.dm_info.chips_factory, ctx->env.manufacturers.chipFactory, sizeof(attr.dm_info.chips_factory));
    strncpy(attr.dm_info.chips_model, ctx->env.manufacturers.chipModel, sizeof(attr.dm_info.chips_model));
    NetClient net(dev);
    Net::Addr addr = {};
    net.GetAddr(addr);
    Net::LinkStat link = {};
    net.GetLinkStat(link);
    strncpy(attr.dm_info.deviceIP, link.lan4 ? addr.ip4 : addr.ip6, sizeof(attr.dm_info.deviceIP));      //设备IP
    strncpy(attr.dm_info.powerSupplyMode, ctx->env.manufacturers.power,
            sizeof(attr.dm_info.powerSupplyMode)); //供电类型，电池
    strncpy(attr.dm_info.romSize, ctx->env.manufacturers.romSize, sizeof(attr.dm_info.romSize));
    strncpy(attr.dm_info.ramSize, ctx->env.manufacturers.ramSize, sizeof(attr.dm_info.ramSize));

    andlink_adapt_init(&attr);
    m_created = true;
    emxlogi("AndLink created\n");
}

void AndLink::Destroy() {
    if (m_created) {
        m_created = false;
        andlink_adapt_uninit();
        emxlogi("AndLink destroyed\n");
    }
}

int AndLink::Reset() {
    if (m_created) {
        return andlink_adapt_reset();
    }
    return 0;
}

int AndLink::SetScanCode(char *userKey, char *gwAddress, char *gwAddress2) {
    if (m_created) {
        return andlink_adapt_setScanCode(userKey, gwAddress, gwAddress2);
    }
    return 0;
}

int AndLink::SetIp(const char *ip) {
    if (m_created) {
        return andlink_adapt_setIP((char *) ip);
    }
    return 0;
}
