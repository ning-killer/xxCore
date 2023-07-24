//
// Created by xiong on 2021/4/30.
//

#ifndef EMX_MediaChipISP_HPP
#define EMX_MediaChipISP_HPP

#include "MediaServerISP.hpp"

namespace Emx {
    class MediaChipISP : public MediaServerISP {
    public:
        MediaChipISP(int32_t chn, MediaServerResource &res) :
                MediaServerISP(chn, res), m_mode(RunModeE::UnDef), m_started(false) {}

        virtual ~MediaChipISP() {};
    public:

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetRunMode(RunModeE mode) override;

        ErrCodeE GetRunMode(RunModeE &mode) override {
            mode = m_mode;
            return ErrCodeE::Success;
        }

//        ErrCodeE SetGrey(bool grey) override;
//
//        ErrCodeE GetGrey(bool &grey) override {
//            grey = m_grey;
//            return ErrCodeE::SUCCESS;
//        }

    protected:

        virtual void StartCollection();

        virtual void StopCollection();

        virtual void Collection();

        virtual ErrCodeE SetExposure(OptModeE expMode, ExpGainE autoGain, ExpGainE manualGain,
                                     uint8_t compensation, int32_t expTime) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetBrightness(uint8_t brightness) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetSharpness(uint8_t sharpness) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetHue(uint8_t hue) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetContrast(uint8_t contrast) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetSaturation(uint8_t saturation) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetDeNoise(uint8_t deNoise) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE
        SetWb(OptModeE wbMode, uint8_t redGain, uint8_t blueGain) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE SetAntiFlicker(uint8_t antiFlickerHz, bool forceAntiFlickerEna);

    protected:
        int m_pipe;
        int m_infoInvMs;
        EuvTimer m_timer;
        RunModeE m_mode;
        WDR_MODE_E m_wdrMode;
//        bool m_grey;
        bool m_started;
    };
}

#endif //EMX_MediaChipISP_HPP
