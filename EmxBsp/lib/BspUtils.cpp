//
// Created by xiong on 2022/6/28.
//

#include "BspUtils.hpp"

using namespace Emx;

BspUtils *BspUtils::m_inst = nullptr;

ErrCodeE BspUtils::Create() {
    m_sem.Create("/BspUtils", 1);
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/bsp.json", Misc::GetAppConfigsDir());
    if (EasyJson::Load(path, m_cfg) != ErrCodeE::Success) {
        emxloge("load bsp cfg failed\n");
    }
    return ErrCodeE::Success;
}

void BspUtils::Lock() { m_sem.Get(); }

void BspUtils::UnLock() { m_sem.Post(); }

Json::Value &BspUtils::GetCfg() { return m_cfg; }

Gpio *BspUtils::CreateGpio(Json::Value &json, const char *name) {
    if (!json.isMember(name)) {
        emxlogw("%s not exist in bsp\n", name);
        return nullptr;
    }
    auto &e = json[name];
    return new Gpio(e["num"].asInt(),
                    (Gpio::DirE) e["dir"].asInt(),
                    (Gpio::ActiveE) e["active"].asInt(),
                    (Gpio::ValueE) e["default"].asInt());
}
