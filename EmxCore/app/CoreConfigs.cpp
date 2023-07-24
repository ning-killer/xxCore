//
// Created by xiong on 2021/7/29.
//

#include "CoreConfigs.hpp"
#include "core/EmxUtils.hpp"

using namespace Emx;

ErrCodeE CoreConfigs::Load() {
    if (!m_cfgPath) {
        printf("not set core configuration path\n");
        return ErrCodeE::Failure;
    }
    auto e = EasyJson::Load(m_cfgPath, m_cfg);
    if (e != ErrCodeE::Success) {
        printf("load [%s] failed with %d\n", m_cfgPath, (int) e);
    }
    return e;
}
