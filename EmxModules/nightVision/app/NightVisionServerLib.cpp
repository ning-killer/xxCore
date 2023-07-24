/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "NightVisionServerBase.hpp"
#include "NightVisionFactory.hpp"
#include "NightVisionServerLib.hpp"

#include <utility>

using namespace Emx;

NightVisionServerLib::NightVisionServerLib() {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/nightVisionCfg.json", Misc::GetAppConfigsDir());
    if (!File::Exist(path)) {
        printf("%s not exist\n", path);
        return;
    }
    Json::Value json;
    if (EasyJson::Load(path, json) == Emx::ErrCodeE::Success) {
        m_server = NightVisionFactory::Make(json["type"].asCString());
    }
}

NightVisionServerLib::~NightVisionServerLib() {
    delete m_server;
}

void NightVisionServerLib::Create(OnCreated cb, bool newThread) {
    m_server->Create(std::move(cb), newThread);
}

void NightVisionServerLib::Destroy() {
    m_server->Destroy();
}
