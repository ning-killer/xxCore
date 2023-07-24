/*
* @Author: xiong
* @Date: 2023/4/7
*/

#include "Ptz.hpp"
#include "MotorGpio.hpp"

using namespace Emx;

Ptz::Ptz() {
    m_motor = nullptr;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/ptz/ptz.json", Misc::GetAppConfigsDir());
    Json::Value root;
    EasyJson::Load(path, root);
    std::string type = root["type"].asString();
    if (type == "MotorGpio")
        m_motor = new MotorGpio;
    else
        emxloge("unknown type %s\n", type.c_str());
}

Ptz::~Ptz() {
    delete m_motor;
}

ErrCodeE Ptz::Init() {
    return m_motor ? m_motor->Init() : ErrCodeE::ResNotExist;
}

void Ptz::DeInit() {
    if (m_motor)
        m_motor->DeInit();
}

ErrCodeE Ptz::MoveStep(Ptz::TypeE type, bool block, int step, int speed) {
    return m_motor ? m_motor->MoveStep(type, block, step, speed) : ErrCodeE::ResNotExist;
}

bool Ptz::IsBusy(Ptz::TypeE type) {
    return m_motor != nullptr && m_motor->IsBusy(type);
}

ErrCodeE Ptz::GetStat(Ptz::TypeE type, bool block, Ptz::Status &status) {
    return m_motor ? m_motor->GetStat(type, block, status) : ErrCodeE::ResNotExist;
}

ErrCodeE Ptz::Reset() {
    return m_motor ? m_motor->Reset() : ErrCodeE::ResNotExist;
}


