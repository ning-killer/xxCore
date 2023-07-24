//
// Created by xiong on 2021/7/29.
//

#ifndef EAPILCOREMEDIA_CORECONFIGS_HPP
#define EAPILCOREMEDIA_CORECONFIGS_HPP

#include "json/json.h"
#include "core/EmxTypeDef.hpp"

namespace Emx {
    class CoreConfigs {
    private:
        CoreConfigs() : m_cfgPath(nullptr) {};

    public:
        static CoreConfigs *GetInst() {
            static CoreConfigs m_inst;
            return &m_inst;
        }

        void SetCfgPath(const char *dir) { m_cfgPath = dir; }

        const char *GetCfgPath() { return m_cfgPath; }

        Json::Value &GetCfg() { return m_cfg; }

        ErrCodeE Load();

    private:
        const char *m_cfgPath;
        Json::Value m_cfg;

    };
}


#endif //EAPILCOREMEDIA_CORECONFIGS_HPP
