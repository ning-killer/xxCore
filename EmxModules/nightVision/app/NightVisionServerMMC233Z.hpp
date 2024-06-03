/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionServerMMC233Z_HPP
#define EMX_NightVisionServerMMC233Z_HPP

#include "NightVisionServerBase.hpp"
#include "EmxMedia.hpp"
#include "EmxBsp.hpp"

namespace Emx {

    class NightVisionServerMMC233Z : public NightVisionServerBase {
    public:
        NightVisionServerMMC233Z() : NightVisionServerBase() {
            memset(&m_ispInfo, 0, sizeof(MediaISP::Info));
        }

        enum AntiShockStatus {
            ToDay = 0,
            ToIr = 1,
        };

    private:
        ErrCodeE CreateSub(Json::Value &json) override;

        void DestroySub() override;

        void DoSwitch(ModeE mode) override;

        void PublishReset() override;

    private:

        void OnAutoModeJudge();

        void Day2IrJudge();

        void Ir2DayJudge();

        void Day2ColorJudge();

        void Color2DayJudge();

        void AntiShockJudge(AntiShockStatus status);

        int GetLockTimer();

        struct Config {
            static const int SenMaxNum = 9;
            int ispChn;
            int adcChn;
            struct {
                struct {
                    int iso;
                } soft;
                int hard;
            } day2ir[SenMaxNum];
            bool day2irUsingHard;
            struct {
                struct {
                    int iso;
                    int redGain;
                    int blueGain;
                } soft;
                int hard;
            } ir2day[SenMaxNum];
            bool ir2dayUsingHard;
            struct {
                struct {
                    int iso;
                } soft;
                int hard;
            } day2color[SenMaxNum];
            bool day2colorUsingHard;
            struct {
                struct {
                    int isoMin;
                    int isoMax;
                } soft;
                int hard;
            } color2day[SenMaxNum];
            bool color2dayUsingHard;
            struct Channel {
                int isp;
                int irCut;
                int whiteLight;
                int irLight;
            };
            std::vector<Channel> chn;
        };

        //防震挡数据结构定义
        struct AntiShock {
            bool enable = false;     //!< 震荡开关
            int unlockTime;          //!< 解锁时间 单位s
            int oscillationInterval; //!< 震荡时间间隔 单位s
            int switchCount;         //!< 连续切换次数
            int lastlockTime;        //!< 上次锁定时间
            int countContinueUnlock; //!< 记录连续解锁次数
            bool islocked;           //!< 是否锁定
            std::vector<int> switchCaches; 
            int timer;
        };
        
    private:
        Config m_cfg;
        MediaClientISPInfo m_isp;
        EuvTimer m_timer;
        MediaISP::Info m_ispInfo;
        Bsp m_bsp;
        int m_debounceCount;
        int m_todayDelay;
        int m_toirDelay;
        AntiShock m_ir2dayAntiShok;
    };

}

#endif //NightVisionServerMMC233Z
