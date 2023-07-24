//
// Created by xiong on 2021/5/21.
//

#ifndef EMX_EmxProtoV2_HPP
#define EMX_EmxProtoV2_HPP

#include "json/json.h"
#include "EmxCore.hpp"

namespace Emx {

    class EmxProtoV2 {
    public:
        EmxProtoV2();

        enum class CmdE : uint16_t {
            ////////////////////////////////基本信息////////////////////////////////
            BaseGet = 30101, //!<获取设备基本信息
            PlatBurningGet = 30102, //!<获取设备平台烧录信息
            PlatBurningSet = 30103, //!<设置设备平台烧录信息
            PlatCfgGet = 30104, //!<获取设备平台配置信息
            PlatCfgSet = 30105, //!<设置设备平台配置信息

            ////////////////////////////////网络////////////////////////////////
            NetMainInfoGet = 30201, //!<获取当前主网络信息
            NetMainInfoSet = 30202, //!<设置当前主网络信息
            NetWifiGet = 30203, //!<获取wifi信息
            NetWifiSet = 30204, //!<设置wifi信息
            NetSupportTypeGet = 30205, //!<获取设备支持的网络类型
            NetSpecTypeInfoGet = 30206, //!<获取指定网络信息
            NetSpecTypeInfoSet = 30207, //!<设置指定网络信息
            NetDnsGet = 30208, //!<获取设备DNS信息
            NetDnsSet = 30209, //!<设置设备DNS信息

            ////////////////////////////////串号SN////////////////////////////////
            SnGet = 30301, //!<获取SN
            SnSet = 30302, //!<设置SN

            ////////////////////////////////视频////////////////////////////////
            VideoEncQue = 30401, //!<获取视频编码参数取值范围
            VideoEncGet = 30402, //!<获取视频编码参数
            VideoEncSet = 30403, //!<设置视频编码参数

            ////////////////////////////////音频////////////////////////////////
            AudioEncQue = 30501, //!<获取音频编码参数取值范围
            AudioEncGet = 30502, //!<获取音频编码参数
            AudioEncSet = 30503, //!<设置音频编码参数
            AudioDecQue = 30504, //!<获取音频解码参数取值范围
            AudioDecGet = 30505, //!<获取音频解码参数
            AudioDecSet = 30506, //!<设置音频解码参数

            ////////////////////////////////音频////////////////////////////////
            IspGet = 30601, //!<获取ISP参数
            IspSet = 30602, //!<设置ISP参数

            ////////////////////////////////缩略图////////////////////////////////
            SnapGet = 30701, //!<获取当前缩略图

            ////////////////////////////////音频播放////////////////////////////////
            AudioPlay = 30801, //!<播放音频文件

            ////////////////////////////////设备操作////////////////////////////////
            Reboot = 30901, //!<重启设备
            Reset = 30902, //!<重置设备
            LogLevelGet = 30903, //!<获取设备日志级别
            LogLevelSet = 30904, //!<设置设备日志级别
            LogGet = 30905, //!<获取日志

            ////////////////////////////////SD卡////////////////////////////////
            SdInfoGet = 31001, //!<获取SD卡信息
            SdFormat = 31002, //!<格式化SD卡
            SdCheck = 31003, //!<检查SD卡读写是否正常

            ////////////////////////////////夜视////////////////////////////////
            NightVisionGet = 31101, //!<获取夜视信息
            NightVisionSet = 31102, //!<设置夜视信息

            ////////////////////////////////时间////////////////////////////////
            TimeGet = 31201, //!<获取设备时间信息
            TimeSet = 31202, //!<设置设备时间信息
            TimeZoneListGet = 31203, //!<获取设备时区列表

            ////////////////////////////////MAC////////////////////////////////
            MacGet = 31301, //!<获取MAC
            MacSet = 31302, //!<设置MAC

            ////////////////////////////////标定////////////////////////////////
            TemplateGet = 31401, //!<获取模板
            TemplateSet = 31402, //!<设置模板
            CenterGet = 31403, //!<获取中心坐标
            CenterSet = 31403, //!<设置中心坐标

            ////////////////////////////////OSD////////////////////////////////
            OsdGet = 31501, //!<获取OSD
            OsdSet = 31502, //!<设置OSD

            ////////////////////////////////IO////////////////////////////////
            IrCutSwitch = 31601, //!<IRCut切换
            LumaGet = 31602, //!<获取光敏ADC&Luma数值
            IrLightGet = 31603, //!<获取红外灯状态
            IrLightSet = 31604, //!<控制红外灯
            WhiteLightGet = 31605, //!<获取白光灯状态
            WhiteLightSet = 31606, //!<控制白光灯
            IndicatorLightGet = 31607, //!<获取指示灯状态
            IndicatorLightSet = 31608, //!<控制指示灯
            PressKeyGet = 31609, //!<获取按键状态
            PressKeySet = 31610, //!<控制按键状态
            ////////////////////////////////事件////////////////////////////////
            EventListGet = 31701,
            EventGlobalGet = 31702,
            EventGlobalSet = 31703,
            EventPluginGet = 31704,
            EventPluginSet = 31705,
            ////////////////////////////////文件烧录////////////////////////////////
            FileBurning = 31801,

            ////////////////////////////////录像////////////////////////////////
            RecordStdGet = 31901, //!<获取录像参数
            RecordStdSet = 31902, //!<设置录像参数
            RecordStdRecordManualGet = 31903, //!<获取手动录像状态
            RecordStdRecordManualSet = 31904, //!<设置手动录像状态
            RecordStdRecordListGet = 31905, //!<获取录像列表
            RecordStdSnapManualTrigger = 31906, //!<触发手动抓图
            RecordStdSnapListGet = 31907, //!<获取抓图列表

            ////////////////////////////////扩展////////////////////////////////
            ExtendCmd = 32000, //!<扩展协议
        };

        /**
         * @brief 处理接收到的命令和数据，并返回结果
         * @param data [in] 需要处理的数据
         * @param size [in] 需要处理的数据大小
         * @param out [out] 返回的处理结果
         */
        void Proc(const char *data, int32_t size, std::string &out);

    private:
        std::map<CmdE, std::function<void(Json::Value &in, Json::Value &out, Json::Value &header)>> m_funcMap;

        void MakeCodeDesc(Json::Value &out, ErrCodeE e, const char *desc) {
            out["code"] = (int) e;
            out["desc"] = desc;
        }

    private:

        void BaseGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PlatBurningGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PlatBurningSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PlatCfgGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PlatCfgSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetMainInfoGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetMainInfoSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetWifiGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetWifiSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetSupportTypeGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetSpecTypeInfoGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetSpecTypeInfoSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetDnsGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NetDnsSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void SnGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void SnSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void VideoEncQue(Json::Value &in, Json::Value &out, Json::Value &header);

        void VideoEncGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void VideoEncSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioEncQue(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioEncGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioEncSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioDecQue(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioDecGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioDecSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IspGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IspSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void SnapGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void AudioPlay(Json::Value &in, Json::Value &out, Json::Value &header);

        void Reboot(Json::Value &in, Json::Value &out, Json::Value &header);

        void Reset(Json::Value &in, Json::Value &out, Json::Value &header);

        void LogLevelGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void LogLevelSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void LogGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void SdInfoGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void SdFormat(Json::Value &in, Json::Value &out, Json::Value &header);

        void SdCheck(Json::Value &in, Json::Value &out, Json::Value &header);

        void NightVisionGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void NightVisionSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void TimeGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void TimeSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void TimeZoneListGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void MacGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void MacSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void TemplateGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void TemplateSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void CenterGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void CenterSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void OsdGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void OsdSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IrCutSwitch(Json::Value &in, Json::Value &out, Json::Value &header);

        void LumaGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IrLightGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IrLightSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void WhiteLightGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void WhiteLightSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IndicatorLightGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void IndicatorLightSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PressKeyGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void PressKeySet(Json::Value &in, Json::Value &out, Json::Value &header);

        void ExtendCmd(Json::Value &in, Json::Value &out, Json::Value &header);

        void EventListGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void EventGlobalGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void EventGlobalSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void EventPluginGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void EventPluginSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdRecordManualGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdRecordManualSet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdRecordListGet(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdSnapManualTrigger(Json::Value &in, Json::Value &out, Json::Value &header);

        void RecordStdSnapListGet(Json::Value &in, Json::Value &out, Json::Value &header);
    };

}

#endif //EMX_EmxProtoV2_HPP
