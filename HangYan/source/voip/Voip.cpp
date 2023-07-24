//
// Created by xiong on 2022/6/21.
//
#include "Voip.hpp"
#include "EmxMedia.hpp"

using namespace Emx;

ErrCodeE Voip::Create(OvdCtx *ctx) {
    if (m_created)
        return ErrCodeE::Success;
    m_voipImpl.Create(ctx);
    m_created = true;
    emxlogi("Voip created\n");
    return ErrCodeE::Success;
}

void Voip::Destroy() {
    if (!m_created)
        return;
    m_voipImpl.Destroy();
    m_created = false;
    emxlogi("Voip destroyed\n");
}
