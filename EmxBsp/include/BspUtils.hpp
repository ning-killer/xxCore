//
// Created by xiong on 2022/6/28.
//

#ifndef EMX_BspUtils_HPP
#define EMX_BspUtils_HPP

#include "EmxCore.hpp"

namespace Emx {
    class BspUtils {
    private:
        BspUtils() {}

    public:

        static BspUtils *GetInst() {
            if (!m_inst) {
                m_inst = new BspUtils;
                m_inst->Create();
            }
            return m_inst;
        }

        void Lock();

        void UnLock();

        Json::Value &GetCfg();

        Gpio *CreateGpio(Json::Value &json, const char *name);

    private:
        ErrCodeE Create();

    private:
        static BspUtils *m_inst;
        Sem m_sem;
        Json::Value m_cfg;
    };
}

#endif //EMX_BspUtils_HPP
