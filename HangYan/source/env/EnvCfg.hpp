//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvCfg_HPP
#define EAPIL_EnvCfg_HPP

#include "EnvBase.hpp"

namespace Emx {

    class EnvCfg : public EnvBase {
    public: 
        enum NightvisionDetectMode {
            person = 0, //人形
            motion = 1, //移动
        };

    public:
        EnvCfg() : EnvBase("cfg") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

    public:
        char bindId[32]; //绑定设备的客户id
        bool trace; //trace enable
        int manual_sleep; //是否手动设置休眠了 0,初始化；1.手动开启休眠；2，手动关闭休眠
        bool led; //Led灯是否打开
        bool rsk_encrypt; //视频是否需要加密
        int net_mode; //1:声波配网 2:二维码配网
        int logSTD; //志输出位置 0:代表不输出  1：输出到标准输出  2：输出到标准出错
        int logLevel; //日志输出级别，详细见OpenAPI枚举值OVDLogLevel
        int logContrl; //实时日志开关 0: 关闭  1：开启
        int callback; //设备是否提供日志写入回调函数 1,写入回调;2,不写入回调
        char log_path[64]; //ovd sdk日志记录目录
        int cnl_cnt; //设备支持多少个channel
        int video_quality; //高标清信息 0,低清 1,标清 2,高清 3,全高清
        bool auto_reboot_on; //是否自动重启
        long long auto_reboot_cycle; //自动维护（重启）的最短周期，单位秒，例如7天自动重启，可以设置为604800>
        char auto_reboot_start[16]; //<必填，可读可写,整型：自动维护（重启）开始时间，格式“HH:MM:SS”>设备可在start到end时间内随机选择一个时间重启>
        char auto_reboot_end[16]; //<必填，可读可写,整型：自动维护（重启）结束时间，格式“HH:MM:SS”>
        char last_reboot[64]; //<必填，只读,字符串型：上次重启时间，格式”YY-MM-DD HH:MM:SS”>
        bool softprobe_on;//是否开启软探针
        int last_upgrade_state;     //上一次升级的状态
        char last_upgrade_time[64];               //<必填，只读,字符串型：最后一次升级时间，格式”YY-MM-DDTHH:MM:SS”>
        bool intelligentNightVision;
        NightvisionDetectMode nightvision_detect_mode;  //触发全彩行为类型分类
    };

}

#endif //EAPIL_EnvCfg_HPP
