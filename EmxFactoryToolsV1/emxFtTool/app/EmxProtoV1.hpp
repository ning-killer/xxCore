//
// Created by xiong on 2021/5/21.
//

#ifndef EMXFCTOOLSV1_EMXProtoV1_HPP
#define EMXFCTOOLSV1_EMXProtoV1_HPP

#include "json/json.h"
#include "EmxCore.hpp"
#include "EmxPlatform.hpp"
#include "libconfig.h"
#include "AgeingInner.hpp"

namespace Emx {

    class EmxProtoV1 {
      public:
        EmxProtoV1();

        enum class CmdE : uint16_t {
            //base info
            SN_GET = 0x2100, //!<获取设备序列号
            SN_SET = 0x2101, //!<设置设备序列号
            TMPL_GET = 0x2102, //!<获取模板信息
            TMPL_SET = 0x2103, //!<设置模板信息
            BASE_GET = 0x2104, //!<获取设备基本信息
            //center
            CENTER_GET = 0x2110, //!<获取中心坐标
            CENTER_SET = 0x2111, //!<设置中心坐标
            //video
            VIDEO_MAJOR_QUE = 0x2120, //!<获取视频编码主码流参数取值范围
            VIDEO_MAJOR_GET = 0x2121, //!<获取视频编码主码流参数
            VIDEO_MAJOR_SET = 0x2122, //!<设置视频编码主码流参数
            VIDEO_MINOR_QUE = 0x2123, //!<获取视频编码子码流参数取值范围
            VIDEO_MINOR_GET = 0x2124, //!<获取视频编码子码流参数
            VIDEO_MINOR_SET = 0x2125, //!<设置视频编码子码流参数
            //audio
            AUDIO_QUE = 0x2130, //!<获取音频参数取值范围
            AUDIO_GET = 0x2131, //!<获取音频参数
            AUDIO_SET = 0x2132, //!<设置音频参数
            //isp
            ISP_QUE = 0x2140, //!<获取ISP参数取值范围
            ISP_GET = 0x2141, //!<获取ISP参数
            ISP_SET = 0x2142, //!<设置ISP参数
            //snap
            SNAP = 0x2150, //!<获取缩略图
            //music
            MUSIC = 0x2160, //!<播放音乐
            //update firmware
            UPDATE = 0x2170, //!<升级固件
            //reboot
            REBOOT = 0x2180, //!<重启
            //reset
            RESET = 0x2190, //!<重置
            //sd info
            SD_INFO = 0x21a0, //!<获取SD卡信息
            SD_FORMAT = 0x21a1, //!<格式化SD卡
            SD_CHECK = 0x21a2, //!<检查SD卡读写是否正常
            //net
            NET_GET = 0x21b0, //!<获取网络信息
            NET_SET = 0x21b1, //!<设置网络信息
            //wifi
            WIFI_GET = 0x21c0, //!<获取wifi信息
            WIFI_SET = 0x21c1, //!<设置wifi信息
            //time
            TIME_GET = 0x21d0, //!<获取时间信息
            TIME_SET = 0x21d1, //!<设置时间信息
            //icr
            ICR_GET = 0x21e0, //!<获取夜视信息
            ICR_SET = 0x21e1, //!<设置夜视信息
            //光敏
            LDS_GET = 0x21e2, //!<获取光敏信息
            LDS_SET = 0x21e3, //!<设置光敏信息
            //OSD
            OSD_GET = 0x2200, //!<获取OSD信息
            OSD_SET = 0x2201, //!<设置OSD信息
            //ptz控制新版接口
            PTZ_GETV2 = 0x2500, //!<获取PTZ信息
            PTZ_SETV2 = 0x2501, //!<获取PTZ信息
            //face
            FACE_GET = 0x2407,  //!<获取人脸信息
            FACE_SET = 0x2408,  //!<设置人脸信息
            FACE_DEL = 0x2409,  //!<删除人脸信息
            //红外功率
            IREDW_GET = 0x2502, //!<获取红外功率信息
            IREDW_SET = 0x2503, //!<设置红外功率信息
            /*工厂相关指令*/
            //设备SN号是否支持加密存储
            ENCRYPT_SPT = 0x5000, //!<设备SN号是否支持加密存储,支持true;不支持false
            //led
            LED_START   = 0x3001, //!<LED 灯光颜色开始闪烁
            LED_STOP    = 0x3002, //!<LED 灯光颜色停止闪烁
            //rst按键检测
            RESET_START = 0x3003, //!<复位按键开始检测开始
            RESET_STOP  = 0x3004, //!<复位按键开始检测结束
            //SPK
            SPK_START   = 0x3005,   //!<喇叭播放开始检测
            SPK_STOP    = 0x3006,   //!<喇叭播放停止检测
            //设置mac地址(wifimac.txt)
            WIFIMAC_GET = 0x3013,   //!<获取烧录到wifimac.txt mac地址
            WIFIMAC_SET = 0x3014,   //!<烧录mac地址到wifimac.txt
            //设置IMEI(DM信息上报)
            IMEI_GET    = 0x3015,   //!<
            IMEI_SET    = 0x3016,   //!<
            //电机设置
            PTZ_SETV1   = 0x3017, //!<电机检测老版接口
            //WIFI信号质量
            WIFI_QTY_GET = 0x3019,
            //带宽设置
            BAND_WIDTH_SET = 0x301f,
            //老化测试
            AGING_GET = 0x301a,
            AGING_SET = 0x5101,
            //IRCUT切换扩展
            IRCUT_EXT_SET = 0x21f1,
            //单板模式设置(设备端关闭红外、关闭白光灯，设备处于强制白天模式)
            SINGLE_BOARD_SET = 0x21f3,
            /*号码烧录相关*/
            //烧录信息清除
            BURNCLEAR_SET = 0x5001,
            //21CN平台号码烧录、获取
            DEV21CN_GET = 0x3020,
            DEV21CN_SET = 0x3021,
            //和家亲1.0平台号码烧录、获取
            DEVHJQV1_GET = 0x3022,
            DEVHJQV1_SET = 0x3023,
            //和家亲2.0(和家望)平台号码烧录、获取
            DEVHJQV2_GET = 0x301d,
            DEVHJQV2_SET = 0x301e,
            //云眼平台号码烧录、获取
            DEVYY_GET = 0x3200,
            DEVYY_SET = 0x3201,
            //浙江电信平台号码烧录、获取
            DEVZJDX_GET = 0x3025,
            DEVZJDX_SET = 0x3026,
            //天翼看店平台号码烧录、获取
            DEVTYKD_GET = 0x4928,
            DEVTYKD_SET = 0x4929,
            //看家宝平台号码烧录、获取
            DEVKJB_GET = 0x3200,
            DEVKJB_SET = 0x3201,
            //国标平台号码烧录、获取
            DEVGB_GET = 0x3100,
            DEVGB_SET = 0x3101,
            //小草平台号码烧录、获取
            DEVXC_GET = 0x4948,
            DEVXC_SET = 0x4949,
            //涂鸦平台号码烧录、获取
            DEVTY_GET = 0x5100,
            DEVTY_SET = 0x5101,
        };

        /**
          * @brief 处理接收到的命令和数据，并返回结果
          * @param cmd [in] 需要处理的命令
          * @param data [in] 需要处理的数据
          * @param size [in] 需要处理的数据大小
          * @param out [out] 返回的处理结果
          */
        void Proc(CmdE cmd, const char *data, int32_t size, std::string &out);

        /**
         * @brief 设置平台
         * 
         * @param pe 
         */
        void SetPlatform(const EmxPlatform::PlatE &pe);

        void BindLoop(EuvLoop *loop) {
            m_loop = loop;
        }

      private:
        std::map<CmdE, std::function<void(Json::Value &in, Json::Value &out)>> m_funcMap;

      private:
        /**
         * @brief  SN号获取
         * @note   凯木金协议 0x5a0e2100
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed", "sn" : "EPNC00000000" }
         */
        void SnGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  SN号设置
         * @note   凯木金协议 0x5a0e2101
         *         请求：{ "code" : 0, "desc" : "successed", "sn" : "EPNC00000000" }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void SnSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  MAC地址获取
         * @note   凯木金协议 0x5a0e3013
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed", "mac" : "00:E0:99:05:67:E3" }
         */
        void WifiMacGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  MAC地址设置
         * @note   凯木金协议 0x5a0e3014
         *         请求：{ "mac" : "00:E0:99:05:67:E3" }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void WifiMacSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  IMEI获取
         * @note   凯木金协议 0x5a0e3015
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed", "imei" : "123456789" }
         */
        void ImeiGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  IMEI设置
         * @note   凯木金协议 0x5a0e3016
         *         请求：{ "imei" : "123456" }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void ImeiSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  喇叭声音播放
         * @note   凯木金协议 0x5a0e3005
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void SpkStart(Json::Value &in, Json::Value &out);

        /**
         * @brief  红外灯控制/IR-CUT设置/夜视切换
         * @note   凯木金协议 0x5a0e21e1
         *         请求：{ "mode": "manual/auto","switch": "off/on" }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void ICRSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  摄像机SD卡读写检测
         * @note   凯木金协议 0x5a0e21a2
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed" }
         */
        void SDCheck(Json::Value &in, Json::Value &out);

        /**
         * @brief  固件版本信息
         * @note   凯木金协议 0x5a0e2104
         *         请求：{ }
         *         应答：{ "code":0,"desc": "successed","dev_type":"ZNIPC-206DJIR",
         *               "sn":"123456","firmware":"20200601_2_1.0.15","hardware":"V1.0" }
         */
        void BaseGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  复位按键检测
         * @note   凯木金协议 0x5a0e3003
         *         请求：{ }
         *         应答：{ “code” : 0, “desc” : “successed” }
         */
        void ResetStart(Json::Value &in, Json::Value &out);

        /**
         * @brief  电机检测
         * @note   凯木金协议 0x5a0e3017
         *         请求：{ "dir" : 0/1/2/3/4/5, "step":20 }
         *         应答：{ “code” : 0, “desc” : “successed” }
         */
        void PtzSetV1(Json::Value &in, Json::Value &out);

        /**
         * @brief  LED灯控制
         * @note   凯木金协议 0x5a0e3001
         *         请求：{ }
         *         应答：{ “code” : 0, “desc” : “successed” }
         */
        void LedStart(Json::Value &in, Json::Value &out);

        /**
         * @brief  WIFI强度获取
         * @note   凯木金协议 0x5a0e3019
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed"， 
         *                "wifi_quality":"100","ip":"192.168.93.130" }
         */
        void WifiQtyGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  获取⽼化结果
         * @note   凯木金协议 0x5a0e301a
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed", "agingState": true/false }
         */
        void AgingGet(Json::Value &in, Json::Value &out);

        /**
         * @brief  获取设备烧录信息
         * @note   凯木金协议 0x5a0e301d
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed", "imei" : "123456789" , 
         *                "mac":"00:CF:E0:36:9A:F4" , "sn":"123456789", "password":"admin123", 
         *                "mediaEncPsd":"admin123", "appkey":"", "pwd":"" }
         */
        void DevHJQV2Get(Json::Value &in, Json::Value &out);

        /**
         * @brief  设置设备烧录信息
         * @note   凯木金协议 0x5a0e301e
         *         请求：{ "code" : 0, "desc" : "successed", "imei" : "123456789" , 
         *                "mac":"00:CF:E0:36:9A:F4" , "sn":"123456789", "password":"admin123", 
         *                "mediaEncPsd":"admin123", "appkey":"", "pwd":"" }
         *         应答：{ “code” : 0, “desc” : “successed” }
         */
        void DevHJQV2Set(Json::Value &in, Json::Value &out);

        /**
         * @brief  设置带宽参数信息
         * @note   凯木金协议 0x5a0e301f
         *         请求：{ "serverIp" : "192.168.11.2" , "intervalTime":60 , "mode": 1 }
         *         应答：
         *              mode为1时: { "code":0,"desc": "successed" }；
         *              mode为2时: { "code":0,"desc": "successed", "serverIp":"", "port":5001 }
         */
        void BandWidthSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  固件是否支持加密
         * @note   凯木金协议 0x5a0e5000
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed"， "encrypt":true/false }
         */
        void EncryptInfo(Json::Value &in, Json::Value &out);

        /**
         * @brief  白光灯、红外灯扩展协议
         * @note   凯木金协议 0x5a0e21f1
         *         请求：{ "light": "IR/WL","mode": "manual/auto","switch": "off/on" }
         *         应答：{ “code” : 0, “desc” : “successed” }
         */
        void IRcutExtSet(Json::Value &in, Json::Value &out);

        /**
         * @brief  光敏阈值检测
         * @note   凯木金协议 0x5a0e21e2
         *         请求：{ }
         *         应答：{ "code" : 0, "desc" : "successed"， "day2night": 10, 
         *                "night2day": 10, "value": 10 }
         */
        void LDSGet(Json::Value &in, Json::Value &out);

        void NetGet(Json::Value &in, Json::Value &out);

        void NetSet(Json::Value &in, Json::Value &out);

        void NetWifiGet(Json::Value &in, Json::Value &out);

        void NetWifiSet(Json::Value &in, Json::Value &out);

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

        void TimeGet(Json::Value &in, Json::Value &out);

        void TimeSet(Json::Value &in, Json::Value &out);

        void ICRGet(Json::Value &in, Json::Value &out);

        void LDSSet(Json::Value &in, Json::Value &out);

        void OSDGet(Json::Value &in, Json::Value &out);

        void OSDSet(Json::Value &in, Json::Value &out);

        void TemplateGet(Json::Value &in, Json::Value &out);

        void TemplateSet(Json::Value &in, Json::Value &out);

        void CenterGet(Json::Value &in, Json::Value &out);

        void CenterSet(Json::Value &in, Json::Value &out);

        void PtzGetV2(Json::Value &in, Json::Value &out);

        void PtzSetV2(Json::Value &in, Json::Value &out);

        void FaceGet(Json::Value &in, Json::Value &out);

        void FaceSet(Json::Value &in, Json::Value &out);

        void FaceDel(Json::Value &in, Json::Value &out);

        void IredwGet(Json::Value &in, Json::Value &out);

        void IredwSet(Json::Value &in, Json::Value &out);

        void LedStop(Json::Value &in, Json::Value &out);

        void ResetStop(Json::Value &in, Json::Value &out);

        void SpkStop(Json::Value &in, Json::Value &out);

        void AgingSet(Json::Value &in, Json::Value &out);

        void BurnInfoClearSet(Json::Value &in, Json::Value &out);

        void SingleBoardSet(Json::Value &in, Json::Value &out);

        void Dev21CNGet(Json::Value &in, Json::Value &out);

        void Dev21CNSet(Json::Value &in, Json::Value &out);

        void DevHJQV1Set(Json::Value &in, Json::Value &out);

        void DevHJQV1Get(Json::Value &in, Json::Value &out);

        void DevYYSet(Json::Value &in, Json::Value &out);

        void DevYYGet(Json::Value &in, Json::Value &out);

        void DevZJDXGet(Json::Value &in, Json::Value &out);

        void DevZJDXSet(Json::Value &in, Json::Value &out);

        void DevTYKDSet(Json::Value &in, Json::Value &out);

        void DevTYKDGet(Json::Value &in, Json::Value &out);

        void DevKJBGet(Json::Value &in, Json::Value &out);

        void DevKJBSet(Json::Value &in, Json::Value &out);

        void DevGBGet(Json::Value &in, Json::Value &out);

        void DevGBSet(Json::Value &in, Json::Value &out);

        void DevXCSet(Json::Value &in, Json::Value &out);

        void DevXCGet(Json::Value &in, Json::Value &out);

        void DevTYGet(Json::Value &in, Json::Value &out);

        void DevTYSet(Json::Value &in, Json::Value &out); 

      private:
        EmxPlatform::PlatE m_pe;
        std::string m_pubkeyPath;
        EuvLoop *m_loop;
    };
}

#endif //EMXFCTOOLSV1_EMXProtoV1_HPP
