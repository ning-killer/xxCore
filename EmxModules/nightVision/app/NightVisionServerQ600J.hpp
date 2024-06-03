/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionServerQ600J_HPP
#define EMX_NightVisionServerQ600J_HPP

#include "NightVisionServerBase.hpp"
#include "EmxMedia.hpp"
#include "EmxBsp.hpp"

namespace Emx {

    class NightVisionServerQ600J : public NightVisionServerBase {
    public:
        NightVisionServerQ600J() : NightVisionServerBase() {
            memset(&m_ispInfo, 0, sizeof(MediaISP::Info));
        }

    private:
        ErrCodeE CreateSub(Json::Value &json) override;

        void DestroySub() override;

        void DoSwitch(ModeE mode) override;

        void PublishReset() override { }

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
                    int lux;
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
                    int Vehicleiso;
                    int lux;
                } soft;
                int hard;
            } day2color[SenMaxNum];
            bool day2colorUsingHard;
            struct {
                struct {
                    int isoMin;
                    int isoMax;
                    int VehicleisoMin;
                    int VehicleisoMax;
                    int luxMax;
                    int luxMin;
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
        int m_todayDelay;
        int m_toirDelay;
    };

}

#endif //EMX_NightVisionServerQ600J_HPP
