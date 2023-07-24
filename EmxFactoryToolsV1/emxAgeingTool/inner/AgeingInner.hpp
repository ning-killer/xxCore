/**
 * @file AgeingInner.hpp
 * @author hening
 * @brief 管理老化服务公共数据声明
 * @date 2023-4-23
 */
#ifndef EMXFACTORYTOOLSV1_AGEINGINNER_HPP
#define EMXFACTORYTOOLSV1_AGEINGINNER_HPP

#include "EmxCore.hpp"

#define AgeingServerMsgId 0x000E0000
#define AGEING_SERVER_MOD 1
#define AGING_FILE  "/root/configs/normal/aging.cfg"
#define STOP_AGEING_AUDIO "didi.aac" 

namespace Emx {

    enum class EmxAgeingModule : uint8_t {
        GetInfo,  //获取老化信息
    };

    enum AgeingStatus {
        Ageing_NoStarted = 0, //老化未开始
        Ageing_Running = 1,   //老化中
        Ageing_Finish = 2,    //老化结束
    };

    enum AgeingResult {
        Ageing_Success = 0, //老化成功
        Ageing_Failed = 1,  //老化失败
        Ageing_Unknown = 2, //未知状态
    };
    struct ZnDebugInfo {
        /* 是否进入工厂 */
        bool factory_mode = false;
        /* 是否启动老化 */
        bool agingtest = false;
        /* 老化时间：单位：秒 */
        int agingtest_time = 0;
        /* 红外切换时间间隔：单位：秒 */
        int ircut_interval = 0;
    };

    struct AgingInfo {
        /* 老化进行时间：单位：分钟 */
        int time = 0;
        AgeingStatus aging_state = Ageing_NoStarted;
        AgeingResult media_state = Ageing_Unknown;
    };
}
#endif //EMXFACTORYTOOLSV1_AGEINGINNER_HPP

