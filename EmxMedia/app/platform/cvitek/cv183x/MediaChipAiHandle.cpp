#include "EmxCore.hpp"
#include "MediaChipAiHandle.hpp"

using namespace Emx;
MediaChipAiHandle *MediaChipAiHandle::m_inst;
MediaChipAiHandle::MediaChipAiHandle() : m_aiHandle(nullptr), m_aiServiceHandle(nullptr) {
    int s32Ret = CVI_AI_CreateHandle(&m_aiHandle);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("handle create failed with %#x!\n", s32Ret);
        return;
    }
    s32Ret = CVI_AI_Service_CreateHandle(&m_aiServiceHandle, m_aiHandle);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("service handle create failed with %#x!\n", s32Ret);
        CVI_AI_DestroyHandle(m_aiHandle);
        return;
    }
    emxlogd("create ai handle %p:%p done\n", m_aiHandle, m_aiServiceHandle);
}

MediaChipAiHandle::~MediaChipAiHandle() {
    CVI_AI_Service_DestroyHandle(m_aiServiceHandle);
    CVI_AI_DestroyHandle(m_aiHandle);
    emxlogd("destroy ai handle done\n");
}