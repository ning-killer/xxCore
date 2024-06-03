/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionServerP42Like_HPP
#define EMX_NightVisionServerP42Like_HPP

#include "NightVisionServerBase.hpp"

namespace Emx {

    class NightVisionServerP42Like : public NightVisionServerBase {
    public:
        NightVisionServerP42Like() : NightVisionServerBase() {}

    private:
        ErrCodeE CreateSub(Json::Value &json) override { return ErrCodeE::OperationNotSupport; }

        void DestroySub() override {}

        void DoSwitch(ModeE mode) override {}

        void PublishReset() override { }
    };

}

#endif //EMX_NightVisionServerP42Like_HPP
