/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionServerNormal_HPP
#define EMX_NightVisionServerNormal_HPP

#include "NightVisionServerBase.hpp"
#include "EmxMedia.hpp"
#include "EmxBsp.hpp"

namespace Emx {

    class NightVisionServerNormal : public NightVisionServerBase {
    public:
        NightVisionServerNormal() : NightVisionServerBase() {
            memset(&m_ispInfo, 0, sizeof(MediaISP::Info));
        }

    private:
        ErrCodeE CreateSub(Json::Value &json) override;

        void DestroySub() override;

        void DoSwitch(ModeE mode) override;

    private:

        void OnAutoModeJudge();

        void Day2IrJudge();

        void Ir2DayJudge();

        void Day2ColorJudge();

        void Color2DayJudge();


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
    private:
        Config m_cfg;
        MediaClientISPInfo m_isp;
        EuvTimer m_timer;
        MediaISP::Info m_ispInfo;
        Bsp m_bsp;
        int m_debounceCount;
    };

}

#endif //EMX_NightVisionServerNormal_HPP
