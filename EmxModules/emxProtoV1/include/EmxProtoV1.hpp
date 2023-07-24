//
// Created by xiong on 2021/5/21.
//

#ifndef EMX_EmxProtoV1_HPP
#define EMX_EmxProtoV1_HPP

#include "json/json.h"
#include "EmxCore.hpp"

namespace Emx {

    class EmxProtoV1 {
    public:
        EmxProtoV1();

        enum class CmdE : uint16_t {
            //base info
            SnGet = 0x2100, //!<获取设备序列号
            SnSet = 0x2101, //!<设置设备序列号
            TmplGet = 0x2102, //!<获取模板信息
            TmplSet = 0x2103, //!<设置模板信息
            BaseGet = 0x2104, //!<获取设备基本信息

            //center
            CenterGet = 0x2110, //!<获取中心坐标
            CenterSet = 0x2111, //!<设置中心坐标

            //video
            VideoMajorQue = 0x2120, //!<获取视频编码主码流参数取值范围
            VideoMajorGet = 0x2121, //!<获取视频编码主码流参数
            VideoMajorSet = 0x2122, //!<设置视频编码主码流参数
            VideoMinorQue = 0x2123, //!<获取视频编码子码流参数取值范围
            VideoMinorGet = 0x2124, //!<获取视频编码子码流参数
            VideoMinorSet = 0x2125, //!<设置视频编码子码流参数

            //audio
            AudioQue = 0x2130, //!<获取音频参数取值范围
            AudioGet = 0x2131, //!<获取音频参数
            AudioSet = 0x2132, //!<设置音频参数

            //isp
            IspQue = 0x2140, //!<获取ISP参数取值范围
            IspGet = 0x2141, //!<获取ISP参数
            IspSet = 0x2142, //!<设置ISP参数

            //snap
            Snap = 0x2150, //!<获取缩略图

            //music
            Music = 0x2160, //!<播放音乐

            //update firmware
            Update = 0x2170, //!<升级固件

            //reboot
            Reboot = 0x2180, //!<重启

            //reset
            Reset = 0x2190, //!<重置

            //sd info
            SdInfo = 0x21a0, //!<获取SD卡信息
            SdFormat = 0x21a1, //!<格式化SD卡
            SdCheck = 0x21a2, //!<检查SD卡读写是否正常

            //net
            NetGet = 0x21b0, //!<获取网络信息
            NetSet = 0x21b1, //!<设置网络信息

            //wifi
            WifiGet = 0x21c0, //!<获取wifi信息
            WifiSet = 0x21c1, //!<设置wifi信息

            //time
            TimeGet = 0x21d0, //!<获取时间信息
            TimeSet = 0x21d1, //!<设置时间信息

            //icr
            IcrGet = 0x21e0, //!<获取夜视信息
            IcrSet = 0x21e1, //!<设置夜视信息

            //光敏
            LdsGet = 0x21e2, //!<获取光敏信息
            LdsSet = 0x21e3, //!<设置光敏信息

            //OSD
            OsdGet = 0x2200, //!<获取OSD信息
            OsdSet = 0x2201, //!<设置OSD信息
        };

        /**
          * @brief 处理接收到的命令和数据，并返回结果
          * @param cmd [in] 需要处理的命令
          * @param data [in] 需要处理的数据
          * @param size [in] 需要处理的数据大小
          * @param out [out] 返回的处理结果
          */
        void Proc(CmdE cmd, const char *data, int32_t size, std::string &out);

    private:
        std::map<CmdE, std::function<void(Json::Value &in, Json::Value &out)>> m_funcMap;

    private:

        void BaseGet(Json::Value &in, Json::Value &out);

        void NetGet(Json::Value &in, Json::Value &out);

        void NetSet(Json::Value &in, Json::Value &out);

        void NetWifiGet(Json::Value &in, Json::Value &out);

        void NetWifiSet(Json::Value &in, Json::Value &out);

        void SnGet(Json::Value &in, Json::Value &out);

        void SnSet(Json::Value &in, Json::Value &out);

        void VideoMajorGet(Json::Value &in, Json::Value &out);

        void VideoMajorSet(Json::Value &in, Json::Value &out);

        void VideoMajorQue(Json::Value &in, Json::Value &out);

        void VideoMinorGet(Json::Value &in, Json::Value &out);

        void VideoMinorSet(Json::Value &in, Json::Value &out);

        void VideoMinorQue(Json::Value &in, Json::Value &out);

        void AudioGet(Json::Value &in, Json::Value &out);

        void AudioSet(Json::Value &in, Json::Value &out);

        void AudioQue(Json::Value &in, Json::Value &out);

        void IspGet(Json::Value &in, Json::Value &out);

        void IspSet(Json::Value &in, Json::Value &out);

        void IspQue(Json::Value &in, Json::Value &out);

        void SnapGet(Json::Value &in, std::string &out);

        void MusicPlay(Json::Value &in, Json::Value &out);

        void Reboot(Json::Value &in, Json::Value &out);

        void Reset(Json::Value &in, Json::Value &out);

        void SDInfo(Json::Value &in, Json::Value &out);

        void SDFormat(Json::Value &in, Json::Value &out);

        void SDCheck(Json::Value &in, Json::Value &out);

        void TimeGet(Json::Value &in, Json::Value &out);

        void TimeSet(Json::Value &in, Json::Value &out);

        void ICRGet(Json::Value &in, Json::Value &out);

        void ICRSet(Json::Value &in, Json::Value &out);

        void LDSGet(Json::Value &in, Json::Value &out);

        void LDSSet(Json::Value &in, Json::Value &out);

        void OSDGet(Json::Value &in, Json::Value &out);

        void OSDSet(Json::Value &in, Json::Value &out);

        void TemplateGet(Json::Value &in, Json::Value &out);

        void TemplateSet(Json::Value &in, Json::Value &out);

        void CenterGet(Json::Value &in, Json::Value &out);

        void CenterSet(Json::Value &in, Json::Value &out);
    };
}

#endif //EMX_EmxProtoV1_HPP
