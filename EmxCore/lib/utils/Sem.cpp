//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Sem.hpp"
#include "core/EmxLog.hpp"
#include <fcntl.h>
#include <cerrno>

using namespace Emx;


ErrCodeE Sem::Create(const char *path, int32_t initValue) {
    snprintf(m_name, EMX_MAX_PATH_SIZE, "%s", path);
    m_semId = sem_open(path, O_CREAT | O_RDWR, 0666, initValue);
    if (m_semId == SEM_FAILED) {
        emxloge("sem_open %d:%s\n", errno, strerror(errno));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

//ErrCodeE Sem::Create(int32_t initValue, int32_t shared) {
//    if (sem_init(&m_semIdS, shared, initValue) == -1) {
//        emxloge("sem_init %d:%s\n", errno, strerror(errno));
//        return ErrCodeE::FAILURE;
//    }
//    m_semId = &m_semIdS;
//    return ErrCodeE::SUCCESS;
//}


ErrCodeE Sem::Wait(int32_t timeoutMs) {
    if (timeoutMs == -1) {
        if (sem_wait(m_semId) == -1) {
            emxloge("sem_wait %d:%s\n", errno, strerror(errno));
            return ErrCodeE::Failure;
        }
    } else {
        timespec ts = {};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += (timeoutMs % 1000) * 1000000;
        ts.tv_nsec = ts.tv_nsec % 1000000000;
        ts.tv_sec += timeoutMs / 1000 + ts.tv_nsec / 1000000000;
        if (sem_timedwait(m_semId, &ts) == -1) {
            if (errno == ETIMEDOUT) {
                emxlogw("sem_timedwait timeout\n");
                return ErrCodeE::Timeout;
            } else {
                emxloge("sem_timedwait %d:%s\n", errno, strerror(errno));
                return ErrCodeE::Failure;
            }
        }
    }
    return ErrCodeE::Success;
}

ErrCodeE Sem::Post() {
    if (sem_post(m_semId) == -1) {
        emxloge("sem_post %d:%s\n", errno, strerror(errno));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}


int32_t Sem::Get() {
    int val;
    if (sem_getvalue(m_semId, &val) == -1) {
        emxloge("sem_getvalue %d:%s\n", errno, strerror(errno));
        return -1;
    }
    return val;
}


void Sem::Destroy() {
    if (m_semId) {
        sem_close(m_semId);
        sem_unlink(m_name);
        m_semId = nullptr;
    }
}
