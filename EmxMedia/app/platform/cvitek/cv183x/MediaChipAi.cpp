#include "EmxCore.hpp"
#include "MediaChipAi.hpp"
#include "MediaConfigs.hpp"
#include "json/json.h"
#include "MediaChipAiPlugMotionDetection.hpp"
#include "MediaChipAiPlugPersonDetection.hpp"

using namespace Emx;

ErrCodeE MediaChipAi::Start() {
    if (m_started)
        return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success)
        return ErrCodeE::Failure;
    if (!m_param.ena)
        return ErrCodeE::Success;

    m_handle = MediaChipAiHandle::GetInst();
    AddAiPlug();
    Json::Value root;
    m_res.configs.GetCfg("ai",root);
    auto &json = root[m_chn];
    StartWork(json);
    m_started = true;
    emxlogi("ai %d done\n",m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAi::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    if (!m_param.ena)
        return ErrCodeE::Success;

    StopWork();
    ClearAiPlug();
    m_started = false;

    return ErrCodeE::Success;
}

ErrCodeE MediaChipAi::SetParam(const MediaAi::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    m_param = param;
    SaveParam();
    return Start();
}

ErrCodeE MediaChipAi::GetParam(MediaAi::Param &param) {
    param = m_param;
    return ErrCodeE::Success;
}

void MediaChipAi::AddAiPlug() {
    m_plugList.push_back(new MediaChipAiPlugMotionDetection(m_chn,m_handle,m_res,m_param));
    m_plugList.push_back(new MediaChipAiPlugPersonDetection(m_chn,m_handle,m_res,m_param));
}

void MediaChipAi::ClearAiPlug() {
    for (auto plug : m_plugList) {
        plug->Destroy();
        delete plug;
    }
    m_plugList.clear();
}

void MediaChipAi::StartWork(Json::Value &json) {
    for (auto plug : m_plugList) {
        plug->Create(json);
        for (auto &plugCb:plug->GetCallBackList()) {
            bool find = false;
            for (auto work : m_workList) {
                if (work->vpssGrp == plugCb.m_vpssGrp && work->vpssChn == plugCb.m_vpssChn) {
                    work->cbFrameList.push_back(plugCb.m_cbFrame);
                    work->cbCompleteList.push_back(plugCb.m_cbComplete);
                    find = true;
                    break;
                }
            }

            if (!find) {
                auto work = new Work(plugCb.m_vpssGrp, plugCb.m_vpssChn);
                work->cbFrameList.push_back(plugCb.m_cbFrame);
                work->cbCompleteList.push_back(plugCb.m_cbComplete);
                m_workList.push_back(work);
            }

        }
    }
    m_quit = false;
    for (auto e : m_workList) {
        e->work.Create(&m_res.loop, e,
                       [this](void *arg) { OnWork(arg); },
                       [this](ErrCodeE e, void *arg) { OnWorkComplete(e, arg); });
        e->work.Run();
    }
}

void MediaChipAi::StopWork() {
    m_quit = true;
    for (auto work : m_workList) {
        work->work.Destroy();
        delete work;
    }
    m_workList.clear();
}

void MediaChipAi::OnWork(void *arg) {
    auto work = (Work *) arg;
    int s32Ret = CVI_SUCCESS;
    VIDEO_FRAME_INFO_S stFrame = {};
    s32Ret = CVI_VPSS_GetChnFrame(work->vpssGrp, work->vpssChn, &stFrame, 500);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VPSS_GetChnFrame failed with %#x\n", m_chn, s32Ret);
        return;
    }

    for (auto &cb : work->cbFrameList) {
        if (cb) {
            cb(stFrame);
        }
    }

    s32Ret = CVI_VPSS_ReleaseChnFrame(work->vpssGrp, work->vpssChn, &stFrame);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VPSS_ReleaseChnFrame failed with %#x\n", m_chn, s32Ret);
    }
}

void MediaChipAi::OnWorkComplete(ErrCodeE e, void *arg) {
    auto work = (Work *) arg;
    for (auto &cb : work->cbCompleteList) {
        if (cb) {
            cb();
        }
    }
    if (!m_quit && e == ErrCodeE::Success)
        work->work.Run();
}