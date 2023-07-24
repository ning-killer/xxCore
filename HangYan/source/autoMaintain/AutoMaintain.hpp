/*
* @Author: xiong
* @Date: 2022/9/30
*/

#ifndef EAPIL_AutoMaintain_HPP
#define EAPIL_AutoMaintain_HPP

#include "OvdCtx.hpp"

namespace Emx {

    class AutoMaintain {
    public:
        using CallBack = std::function<void()>;

        ErrCodeE Create(OvdCtx *ctx, CallBack cb);

        void Destroy();

        void UpdateMaintainTime();

    private:
        int64_t CalculateMaintainTime();

    private:
        bool m_created;
        EuvTimer m_timer;
        OvdCtx *m_ctx;
        CallBack m_cb;
    };

}

#endif //EAPIL_AutoMaintain_HPP
