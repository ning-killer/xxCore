//
// Created by xiong on 2022/6/21.
//

#ifndef EAPIL_Voip_HPP
#define EAPIL_Voip_HPP

#include "EmxCore.hpp"
#include "OvdCtx.hpp"
#include "VoipImpl.hpp"

namespace Emx {

    class Voip {
    public:
        Voip() : m_created(false) {}

        virtual ~Voip() {}

        ErrCodeE Create(OvdCtx *ctx);

        void Destroy();

    private:
        bool m_created;
        VoipImpl m_voipImpl;
    };
}

#endif //EAPIL_Voip_HPP
