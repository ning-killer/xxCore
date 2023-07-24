//
// Created by cx on 2023/6/27.
//

#include "MediaChipAiPlugMotionDetection.hpp"
#include "MediaChipAiPlugFdPdDetection.hpp"
#include "MediaChipFDPFWithAi.hpp"

using namespace Emx;

ErrCodeE MediaChipFDPDWithAi::SetParam(const MediaAi::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }

    SetFaceAlgorithmParam(param);
    m_param = param;
    SaveParam();
    return ErrCodeE::Success;
}

int MediaChipFDPDWithAi::SetFaceAlgorithmParam(const MediaAi::Param &param) {
    MediaChipAiPlugFdPdDetection *prt = NULL;
    for (auto plug: m_plugList) {
        prt = dynamic_cast<MediaChipAiPlugFdPdDetection *>(plug);
        if (prt != NULL)
            break;
    }
    if (prt != NULL) {
        prt->ConfigAiParam(param);
    }

    return 0;
}

void MediaChipFDPDWithAi::AddAiPlug() {
    m_plugList.push_back(new MediaChipAiPlugMotionDetection(m_chn, m_handle, m_res, m_param));
    m_plugList.push_back(new MediaChipAiPlugFdPdDetection(m_chn, m_handle, m_res, m_param));
}



