//
// Created by xiong on 2022/6/27.
//

#ifndef EMX_EmxBsp_HPP
#define EMX_EmxBsp_HPP

#include "EmxCore.hpp"

namespace Emx {
    //通道号为-1代表默认通道
    class BspBase {
    public:
        BspBase() :
                adc(std::make_shared<Adc>()),
                irCut(std::make_shared<IrCut>()),
                irLight(std::make_shared<IrLight>()),
                whiteLight(std::make_shared<WhiteLight>()),
                keyCall(std::make_shared<KeyCall>()),
                keyReset(std::make_shared<KeyReset>()),
                spk(std::make_shared<Spk>()),
                rca(std::make_shared<Rca>()),
                wifi(std::make_shared<Wifi>()),
                mobile(std::make_shared<Mobile>()),
                led(std::make_shared<Led>()),
                updateIndicator(std::make_shared<UpdateIndicator>()) {}

        class GpioLike {
        public:
            GpioLike() {}

            virtual ~GpioLike() {}

            virtual ErrCodeE Set(bool ena) { return ErrCodeE::OperationNotSupport; }

            virtual bool Get() { return false; }
        };

        class Adc {
        public:
            Adc() {}

            virtual ~Adc() {}

            int GetLumaValue() { return GetLumaValue(-1); }

            virtual int GetLumaValue(int chn) { return -1; }

            int GetAdcValue() { return GetAdcValue(-1); }

            virtual int GetAdcValue(int chn) { return -1; }
        };

        std::shared_ptr<Adc> adc;

        class IrCut {
        public:
            IrCut() {}

            virtual ~IrCut() {}

            ErrCodeE SwitchToNight(bool ena) { return SwitchToNight(ena, -1); }

            virtual ErrCodeE SwitchToNight(bool ena, int chn) { return ErrCodeE::OperationNotSupport; }
        };

        std::shared_ptr<IrCut> irCut;

        class IrLight {
        public:
            IrLight() {}

            virtual ~IrLight() {}

            ErrCodeE SetEna(bool ena) { return SetEna(ena, -1); }

            virtual ErrCodeE SetEna(bool ena, int chn) { return ErrCodeE::OperationNotSupport; }

            bool GetEna() { return GetEna(-1); }

            virtual bool GetEna(int chn) { return false; }

            ErrCodeE SetLuma(int luma) { return SetLuma(luma, -1); }

            virtual ErrCodeE SetLuma(int luma, int chn) { return ErrCodeE::OperationNotSupport; }

            int GetLuma() { return GetLuma(-1); }

            virtual int GetLuma(int chn) { return -1; }
        };

        std::shared_ptr<IrLight> irLight;

        class WhiteLight {
        public:
            static const int LumaMinValue = 0;
            static const int LumaMaxValue = 1000;

            WhiteLight() {}

            virtual ~WhiteLight() {}

            ErrCodeE SetEna(bool ena) { return SetEna(ena, -1); }

            virtual ErrCodeE SetEna(bool ena, int chn) { return ErrCodeE::OperationNotSupport; }

            bool GetEna() { return GetEna(-1); }

            virtual bool GetEna(int chn) { return false; }

            ErrCodeE SetLuma(int luma) { return SetLuma(luma, -1); }

            virtual ErrCodeE SetLuma(int luma, int chn) { return ErrCodeE::OperationNotSupport; }

            int GetLuma() { return GetLuma(-1); }

            virtual int GetLuma(int chn) { return -1; }
        };

        std::shared_ptr<WhiteLight> whiteLight;

        class SoundCheck : public GpioLike {
        public:
            SoundCheck() : GpioLike() {}
        };

        std::shared_ptr<SoundCheck> SoundCheck;

        class KeyCall : public GpioLike {
        public:
            KeyCall() : GpioLike() {}
        };

        std::shared_ptr<KeyCall> keyCall;

        class KeyReset : public GpioLike {
        public:
            KeyReset() : GpioLike() {}
        };

        std::shared_ptr<KeyReset> keyReset;

        class Spk : public GpioLike {
        public:
            Spk() : GpioLike() {}
        };

        std::shared_ptr<Spk> spk;

        class Rca : public GpioLike {
        public:
            Rca() : GpioLike() {}
        };

        std::shared_ptr<Rca> rca;

        class Wifi : public GpioLike {
        public:
            Wifi() : GpioLike() {}
        };

        std::shared_ptr<Wifi> wifi;

        class Mobile : public GpioLike {
        public:
            Mobile() : GpioLike() {}
        };

        std::shared_ptr<Mobile> mobile;

        class Led {
        public:
            Led() {}

            virtual ~Led() {}

            virtual ErrCodeE Set(bool ena, int chn) { return ErrCodeE::OperationNotSupport; }

            virtual bool Get(int chn) { return false; }

            virtual int GetChnsNum() const { return 0; }
        };

        std::shared_ptr<Led> led;

        class UpdateIndicator {
        public:
            UpdateIndicator() {}

            virtual ~UpdateIndicator() {}

            virtual ErrCodeE Start() { return ErrCodeE::OperationNotSupport; }

            virtual ErrCodeE Stop() { return ErrCodeE::OperationNotSupport; }
        };

        std::shared_ptr<UpdateIndicator> updateIndicator;
    };

    class Bsp : public BspBase {
    public:
        Bsp();

        virtual ~Bsp();
    };
}

#endif //EMX_EmxBsp_HPP
