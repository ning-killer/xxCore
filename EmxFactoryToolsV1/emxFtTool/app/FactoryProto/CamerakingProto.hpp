//
// Created by hening on 2023/4/19.
//

#ifndef EMXFCTOOLSV1_CAMERAKING_PROTOCONTROL_HPP
#define EMXFCTOOLSV1_CAMERAKING_PROTOCONTROL_HPP

#include "json/json.h"

namespace Emx {
    class CameraKingProto {
      public:
        void MakeCodeDesc(Json::Value &out, ErrCodeE e, const char *desc);
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
    };
}

#endif //EMXFCTOOLSV1_CAMERAKING_PROTOCONTROL_HPP
