/**
 * @file AgeingCtx.hpp
 * @author hening
 * @brief 老化控制中心
 * @date 2023-4-20
 */

#ifndef EMXFACTORYTOOLSV1_AGEINGCTX_HPP
#define EMXFACTORYTOOLSV1_AGEINGCTX_HPP

#include "AgeingInner.hpp"
#include "EmxCore.hpp"
#include <mutex>
using namespace std;

namespace Emx {
    class AgeingCtx {
      public:
        AgeingCtx(EuvLoop *loop);
        virtual ~AgeingCtx();

      public:
        void Start();
        void Stop();

        /**
         * @brief  获取老化配置信息。
         * @param  info [in|out] 老化配置信息AgingInfo。
         * @return 结果错误码。
         */
        ErrCodeE LoadAgeingInfo(AgingInfo &info);

        /**
         * @brief  获取工厂配置信息。
         * @param  path [in] zn_debug工厂配置文件路径。
         * @return 结果错误码。
         */
        ErrCodeE LoadZnDebugInfo(const char* path);

      private:
        void StartLedTest();
        void StartVoiceTest();
        void StartIrcutTest();
        void StartPtzTest();
        bool GetIsStop();
        void SetIsStop(const bool is_stop);
        ErrCodeE InitAgeingFile(const char* path);
        ErrCodeE SaveAgeingInfo(const AgingInfo &info);
        void ResetAgeingInfo();

      private:
        EuvLoop *m_loop = nullptr;
        EuvTimer m_timer;
        ZnDebugInfo m_zndebug_info;
        AgingInfo m_aging_info;
        bool m_started;
        bool m_isstop;
        mutex m_stopflag_locker;
        mutex m_agingfile_locker;
        EuvWork m_led_work;
        EuvWork m_voice_work;
        EuvWork m_ircut_work;
        EuvWork m_ptz_work;
    };

}

#endif //EMXFACTORYTOOLSV1_AGEINGCTX_HPP
