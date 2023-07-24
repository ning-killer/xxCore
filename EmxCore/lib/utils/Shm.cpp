//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Shm.hpp"
#include "core/EmxLog.hpp"
#include <sys/shm.h>
#include <fcntl.h>

using namespace Emx;


void *Shm::Create(int key, int32_t size) {
    if (m_shmPtr)
        return m_shmPtr;
//    m_isMaster = isMaster;
    m_shmId = shmget(key, size, 0666 | IPC_CREAT);
    if (m_shmId == -1) {
        emxloge("shmget error %d:%s\n", errno, strerror(errno));
    } else {
        m_shmPtr = shmat(m_shmId, nullptr, 0);
        if (m_shmPtr == (void *) -1) {
            emxloge("shmat failed %d:%s\n", errno, strerror(errno));
//            if (m_isMaster) {
//                if (shmctl(m_shmId, IPC_RMID, nullptr) == -1) {
//                    emxloge("shmctl(IPC_RMID) failed %d:%s\n", errno, strerror(errno));
//                }
//            }
            m_shmPtr = nullptr;
        }
    }
    return m_shmPtr;
}

void *Shm::Create(int key) {
    if (m_shmPtr)
        return m_shmPtr;
//    m_isMaster = isMaster;
    m_shmId = shmget(key, 0, 0666);
    if (m_shmId == -1) {
        emxlogw("shmget error %d:%s\n", errno, strerror(errno));
    } else {
        m_shmPtr = shmat(m_shmId, nullptr, 0);
        if (m_shmPtr == (void *) -1) {
            emxloge("shmat failed %d:%s\n", errno, strerror(errno));
//            if (m_isMaster) {
//                if (shmctl(m_shmId, IPC_RMID, nullptr) == -1) {
//                    emxloge("shmctl(IPC_RMID) failed %d:%s\n", errno, strerror(errno));
//                }
//            }
            m_shmPtr = nullptr;
        }
    }
    return m_shmPtr;
}

void Shm::Destroy() {
    if (m_shmPtr) {
        if (shmdt(m_shmPtr) == -1) {
            emxloge("shmdt failed %d:%s\n", errno, strerror(errno));
        }
//        if (m_isMaster) {
//            if (shmctl(m_shmId, IPC_RMID, nullptr) == -1) {
//                emxloge("Log shmctl(IPC_RMID) failed %d:%s\n", errno, strerror(errno));
//            }
//        }
        m_shmPtr = nullptr;
    }
}

int32_t Shm::GetSize() {
    if (!m_shmPtr)
        return -1;
    shmid_ds buf = {};
    int ret = shmctl(m_shmId, IPC_STAT, &buf);
    if (ret == -1) {
        emxloge("shmctl shm error");
        return -1;
    }
    return (int32_t) buf.shm_segsz;
}