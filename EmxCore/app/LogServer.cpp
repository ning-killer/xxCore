//
// Created by xiong on 2021/7/19.
//

#include <unistd.h>
#include <sys/un.h>
#include <cstdlib>
#include <utility>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include "LogInner.hpp"
#include "LogServer.hpp"
#include "CoreConfigs.hpp"

static char buffer[EMX_LOG_MAX_SIZE];
using namespace Emx;

static void SetRcvSndBuf(int32_t sock, int32_t rcvBuf, int32_t sndBuf) {
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBuf, sizeof(int)) == -1)
        printf("SO_RCVBUF error %d:%s\n", errno, strerror(errno));

    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sndBuf, sizeof(int)) == -1)
        printf("SO_SNDBUF error %d:%s\n", errno, strerror(errno));
}

static int32_t CreateSock(const char *path) {
    int sock;
    sockaddr_un addr = {};
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    sprintf(addr.sun_path, "%s", path);
    unlink(path);
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bind(sock, (struct sockaddr *) &addr, sizeof(sockaddr_un)) < 0) {
        perror("bind socket failed");
        close(sock);
        sock = -1;
    }
    SetRcvSndBuf(sock, EMX_LOG_MAX_SIZE * 2, EMX_LOG_MAX_SIZE * 2);
    return sock;
}

LogServer::LogServer() {
    memset(&m_cfg, 0, sizeof(m_cfg));
    m_output.ena = CoreConfigs::GetInst()->GetCfg()["output"].asBool();
    if (m_output.ena) {
        m_cfg.shared.async = true;
        m_cfg.shared.level = Log::LevelE::Debug;
    } else {
        auto &json = CoreConfigs::GetInst()->GetCfg()["log"];
        m_cfg.shared.async = json["shared"]["async"].asBool();
        m_cfg.shared.level = (Log::LevelE) json["shared"]["level"].asInt();
        m_cfg.normal.maxSizeKB = json["normal"]["maxSizeKB"].asInt();
        strncpy(m_cfg.normal.path, json["normal"]["path"].asCString(), sizeof(m_cfg.normal.path) - 1);
        m_cfg.flash.maxSizeKB = json["flash"]["maxSizeKB"].asInt();
        strncpy(m_cfg.flash.path, json["flash"]["path"].asCString(), sizeof(m_cfg.flash.path) - 1);
    }
    m_normal.fp = nullptr;
    m_flash.fp = nullptr;
    m_shared = nullptr;
    m_created = false;
}

LogServer::~LogServer() {
    if (m_shared) {
        if (shmdt(m_shm) == -1) {
            fprintf(stderr, "shmdt failed\n");
        }
    }
}

void LogServer::Create() {
    if (m_created)
        return;
    CreateOutput();
    CreateShared();
    CreateLogFile();

    m_loop = (uv_loop_t *) malloc(sizeof(uv_loop_t));
    uv_loop_init(m_loop);
    int sock;

    uv_udp_init(m_loop, &m_normal.sock);
    if ((sock = CreateSock(EMX_LOG_DOMAIN_SERVER_NORMAL)) < 0)
        return;
    uv_udp_open(&m_normal.sock, sock);
    m_normal.sock.data = this;
    uv_udp_recv_start(&m_normal.sock, OnAlloc, OnRecvNormalLog);

    uv_udp_init(m_loop, &m_flash.sock);
    if ((sock = CreateSock(EMX_LOG_DOMAIN_SERVER_FLASH)) < 0)
        return;
    uv_udp_open(&m_flash.sock, sock);
    m_flash.sock.data = this;
    uv_udp_recv_start(&m_flash.sock, OnAlloc, OnRecvFlashLog);

    uv_udp_init(m_loop, &m_ctrl);
    if ((sock = CreateSock(EMX_LOG_DOMAIN_SERVER_CTRL)) < 0)
        return;
    uv_udp_open(&m_ctrl, sock);
    m_ctrl.data = this;
    uv_udp_recv_start(&m_ctrl, OnAlloc, OnRecvCtrl);

    m_quit.data = this;
    uv_async_init(m_loop, &m_quit, OnQuit);
    int err = uv_thread_create(&m_thread, [](void *arg) {
        auto obj = (LogServer *) arg;
        prctl(PR_SET_NAME, "EmxLogServer");
        uv_run(obj->m_loop, UV_RUN_DEFAULT);
        uv_loop_close(obj->m_loop);
        free(obj->m_loop);
    }, this);
    if (err) {
        printf("EmxLogServer thread create failed : %s\n", uv_strerror(err));
        return;
    }
    m_created = true;
}

void LogServer::Destroy() {
    if (m_created) {
        uv_async_send(&m_quit);
        uv_thread_join(&m_thread);
        DestroyOutput();
        m_created = false;
    }
}

void LogServer::OnQuit(uv_async_t *handle) {
    auto obj = (LogServer *) handle->data;
    uv_close((uv_handle_t *) &obj->m_quit, nullptr);
    uv_close((uv_handle_t *) &obj->m_ctrl, nullptr);
    uv_close((uv_handle_t *) &obj->m_normal.sock, nullptr);
    uv_close((uv_handle_t *) &obj->m_flash.sock, nullptr);
}

void LogServer::CreateLogFileChannel(LogServer::Channel &chn, Log::Config::Channel &cfg) {
    DestroyLogFileChannel(chn);
    if (cfg.path[0] == 0)
        return;
    if (access(cfg.path, 0) == 0) {
        chn.fp = fopen(cfg.path, "a+");
        if (chn.fp)
            printf("Emx log reopen %s success\n", cfg.path);
    } else {
        chn.fp = fopen(cfg.path, "w+");
        if (chn.fp)
            printf("Emx log create %s success\n", cfg.path);
    }
    if (!chn.fp)
        printf("Emx log open %s failed !!!\n", cfg.path);
    struct stat st = {};
    stat(cfg.path, &st);
    chn.size = st.st_size;
}

void LogServer::DestroyLogFileChannel(LogServer::Channel &chn) {
    if (chn.fp) {
        fclose(chn.fp);
        chn.fp = nullptr;
    }
}

void LogServer::CreateLogFile() {
    CreateLogFileChannel(m_normal, m_cfg.normal);
    CreateLogFileChannel(m_flash, m_cfg.flash);
}

void LogServer::DestroyLogFile() {
    DestroyLogFileChannel(m_normal);
    DestroyLogFileChannel(m_flash);
}

void LogServer::OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = buffer;
    buf->len = EMX_LOG_MAX_SIZE;
}

void LogServer::CreateShared() {
    m_shared = nullptr;
    m_shm = nullptr;
    m_shmId = shmget(EMX_LOG_SHARED_MEM_KEY, sizeof(Log::Config::Shared), 0666 | IPC_CREAT);
    if (m_shmId == -1) {
        printf("shmget error %d:%s\n", errno, strerror(errno));
    } else {
        m_shm = shmat(m_shmId, nullptr, 0);
        if (m_shm == (void *) -1) {
            printf("shmat failed\n");
//            if (shmctl(m_shmId, IPC_RMID, nullptr) == -1) {
//                printf("Log shmctl(IPC_RMID) failed\n");
//            }
        } else {
            m_shared = (Log::Config::Shared *) m_shm;
            m_shared->level = m_cfg.shared.level;
            m_shared->async = m_cfg.shared.async;
        }
    }
}

void LogServer::OnRecvFlashLog(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
                               unsigned int flags) {
    if (nread <= 0)
        return;
    auto obj = (LogServer *) handle->data;
    if (obj->m_shared && obj->m_shared->async)
        printf("%s", buf->base);
    if (obj->m_output.ena)
        obj->SendOutput(buf->base, (int) buf->len);
    if (obj->m_flash.fp) {
        fprintf(obj->m_flash.fp, "%s", buf->base);
        fflush(obj->m_flash.fp);
        obj->m_flash.size += nread;
        if (obj->m_cfg.flash.maxSizeKB > 0) {
            if (obj->m_flash.size > (int64_t) obj->m_cfg.flash.maxSizeKB * 1024) {
                obj->m_flash.size = 0;
                fseek(obj->m_flash.fp, 0, SEEK_SET);
            }
        }
    }
}

void LogServer::OnRecvNormalLog(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
                                unsigned int flags) {
    if (nread <= 0)
        return;
    auto obj = (LogServer *) handle->data;
    if (obj->m_shared && obj->m_shared->async)
        printf("%s", buf->base);
    if (obj->m_output.ena)
        obj->SendOutput(buf->base, (int) buf->len);
    if (obj->m_normal.fp) {
        fprintf(obj->m_normal.fp, "%s", buf->base);
        fflush(obj->m_normal.fp);
        obj->m_normal.size += nread;
        if (obj->m_cfg.normal.maxSizeKB > 0) {
            if (obj->m_normal.size > (int64_t) obj->m_cfg.normal.maxSizeKB * 1024) {
                obj->m_normal.size = 0;
                fseek(obj->m_normal.fp, 0, SEEK_SET);
            }
        }
    }
}

void LogServer::CmdRespResult(LogInner::CmdE cmd, ErrCodeE e, const struct sockaddr *addr,
                              void *data, int32_t size) {
    int off = 0;
    buffer[off++] = (uint8_t) cmd;
    memcpy(buffer + off, &e, sizeof(ErrCodeE));
    off += sizeof(ErrCodeE);
    if (size && data) {
        memcpy(buffer + off, data, size);
        off += size;
    }
    sendto(m_ctrl.io_watcher.fd, buffer, off, 0, addr, sizeof(sockaddr_un));
}

void LogServer::OnRecvCtrl(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
                           unsigned int flags) {
    if (nread <= 0)
        return;
    auto obj = (LogServer *) handle->data;
    auto cmd = (LogInner::CmdE) buf->base[0];
    printf("%s:%d:cmd=%#x\n", __FUNCTION__, __LINE__, (uint32_t) cmd);
    switch (cmd) {
        case LogInner::CmdE::ConfigGet:
            if (obj->m_shared) {
                obj->m_cfg.shared.level = obj->m_shared->level;
                obj->m_cfg.shared.async = obj->m_shared->async;
            }
            obj->CmdRespResult(cmd, ErrCodeE::Success, addr, &obj->m_cfg, sizeof(obj->m_cfg));
            break;
        case LogInner::CmdE::ConfigSet:
            memcpy(&obj->m_cfg, buf->base + 1, sizeof(obj->m_cfg));
            if (obj->m_shared) {
                obj->m_shared->level = obj->m_cfg.shared.level;
                obj->m_shared->async = obj->m_cfg.shared.async;
            }
            obj->CreateLogFile();
            obj->CmdRespResult(cmd, ErrCodeE::Success, addr);
            break;
        case LogInner::CmdE::Start:
            obj->CreateLogFile();
            obj->CmdRespResult(cmd, ErrCodeE::Success, addr);
            break;
        case LogInner::CmdE::Stop:
            obj->DestroyLogFile();
            obj->CmdRespResult(cmd, ErrCodeE::Success, addr);
            break;
        default:
            return;
    }
}

static void CreateAddrUn(const char *path, sockaddr_un *addr) {
    memset(addr, 0, sizeof(sockaddr_un));
    addr->sun_family = AF_UNIX;
    sprintf(addr->sun_path, "%s", path);
}

void LogServer::CreateOutput() {
    if (!m_output.ena)
        return;
    m_output.sock = CreateSock(EMX_LOG_DOMAIN_SERVER_EXT);
    SetRcvSndBuf(m_output.sock, 1024, EMX_LOG_MAX_SIZE * 2);
    char path[EMX_MAX_PATH_SIZE] = {};
    sprintf(path, "%sExt", EMX_LOG_DOMAIN_CLIENT_PREFIX);
    CreateAddrUn(path, &m_output.address);
}

void LogServer::DestroyOutput() {
    if (!m_output.ena)
        return;
    close(m_output.sock);
}

void LogServer::SendOutput(const char *log, int size) {
    sendto(m_output.sock, log, size, MSG_DONTWAIT, (sockaddr *) &m_output.address, sizeof(m_output.address));
}
