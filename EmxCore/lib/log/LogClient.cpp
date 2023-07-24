//
// Created by xiong on 2021/7/19.
//

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <cerrno>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "core/EmxLog.hpp"
#include "LogInner.hpp"

using namespace Emx;
Log emxLog;
static const char *LevelName[] = {(char *) "crt", (char *) "err", (char *) "wrn",
                                  (char *) "inf", (char *) "dbg", (char *) "tra"};

/* 31, red. 32, green. 33, yellow. 34, blue. 35, magenta. 36, cyan. 37, white. */
static const char *LevelColor[] = {(char *) "[1;35m", (char *) "[1;31m",
                                   (char *) "[1;33m", (char *) "[1;32m",
                                   (char *) "[1;36m", (char *) "[1;37m"};

static void SetRcvSndBuf(int32_t sock, int32_t rcvBuf, int32_t sndBuf) {
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBuf, sizeof(int)) == -1)
        printf("SO_RCVBUF error %d:%s\n", errno, strerror(errno));

    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sndBuf, sizeof(int)) == -1)
        printf("SO_SNDBUF error %d:%s\n", errno, strerror(errno));
}

static void SetRcvSndTimeout(int32_t sock, int32_t rcv, int32_t snd) {
    timeval tv_rcv = {rcv / 1000, (rcv % 1000) * 1000};
    timeval tv_snd = {snd / 1000, (snd % 1000) * 1000};

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv_rcv, sizeof(tv_rcv)) == -1)
        printf("SO_RCVTIMEO error %d:%s\n", errno, strerror(errno));

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv_snd, sizeof(tv_snd)) == -1)
        printf("SO_SNDTIMEO error %d:%s\n", errno, strerror(errno));
}

static void CreateAddrUn(const char *path, sockaddr_un *addr) {
    memset(addr, 0, sizeof(sockaddr_un));
    addr->sun_family = AF_UNIX;
    sprintf(addr->sun_path, "%s", path);
}

static int32_t CreateSock(const char *path) {
    int sock;
    sockaddr_un addr = {};
    CreateAddrUn(path, &addr);
    unlink(path);
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bind(sock, (struct sockaddr *) &addr, sizeof(sockaddr_un)) < 0) {
        perror("bind socket failed");
        close(sock);
        sock = -1;
    }
    return sock;
}

void Log::CreateChannel() {
    char tmp[EMX_MAX_PATH_SIZE];
    int32_t res = readlink("/proc/self/exe", tmp, sizeof(tmp) - 1);
    if (res == -1) {
        printf("Emx log get current app name failed\n");
        return;
    }
    tmp[res] = '\0';
    const char *name = strrchr(tmp, '/');
    name == nullptr ? name = tmp : name += 1;

    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, EMX_MAX_PATH_SIZE, "%s%sCtrl", EMX_LOG_DOMAIN_CLIENT_PREFIX, name);
    m_ctrl.sock = CreateSock(path);
    SetRcvSndBuf(m_ctrl.sock, 1024, EMX_LOG_MAX_SIZE * 2);
    SetRcvSndTimeout(m_ctrl.sock, 5000, 5000);
    CreateAddrUn(EMX_LOG_DOMAIN_SERVER_CTRL, &m_ctrl.address);

    snprintf(path, EMX_MAX_PATH_SIZE, "%s%sNormal", EMX_LOG_DOMAIN_CLIENT_PREFIX, name);
    m_normal.sock = CreateSock(path);
    SetRcvSndBuf(m_normal.sock, 1024, EMX_LOG_MAX_SIZE * 8);
    SetRcvSndTimeout(m_normal.sock, 5000, 5000);
    CreateAddrUn(EMX_LOG_DOMAIN_SERVER_NORMAL, &m_normal.address);

    snprintf(path, EMX_MAX_PATH_SIZE, "%s%sFlash", EMX_LOG_DOMAIN_CLIENT_PREFIX, name);
    m_flash.sock = CreateSock(path);
    SetRcvSndBuf(m_flash.sock, 1024, EMX_LOG_MAX_SIZE * 2);
    SetRcvSndTimeout(m_flash.sock, 5000, 5000);
    CreateAddrUn(EMX_LOG_DOMAIN_SERVER_FLASH, &m_flash.address);
}

void Log::DestroyChannel() {
    close(m_ctrl.sock);
    close(m_normal.sock);
    close(m_flash.sock);
}

Log::Log() {
    if (access(EMX_DIR, 0))
        mkdir(EMX_DIR, 0777);
    if (access(EMX_DOMAIN_DIR, 0))
        mkdir(EMX_DOMAIN_DIR, 0777);
    if (access(EMX_LOG_DOMAIN_DIR, 0))
        mkdir(EMX_LOG_DOMAIN_DIR, 0777);

    CreateChannel();
    m_shared = nullptr;
    m_shm = nullptr;
    m_shmId = shmget(EMX_LOG_SHARED_MEM_KEY, sizeof(Config::Shared), 0666 | IPC_CREAT);
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
            m_shared = (Config::Shared *) m_shm;
        }
    }
}

Log::~Log() {
    DestroyChannel();
    if (m_shared) {
        if (shmdt(m_shm) == -1) {
            fprintf(stderr, "shmdt failed\n");
        }
//        if (shmctl(m_shmId, IPC_RMID, nullptr) == -1) {
//            printf("Log shmctl(IPC_RMID) failed\n");
//        }
    }
}

void Log::Print(LevelE level, const char *file, const char *func, int32_t line,
                const char *fmt, ...) {
    va_list args;
    if (m_shared && (level > m_shared->level))
        return;
    const char *s = strrchr(file, '/');
    s == nullptr ? s = file : s += 1;
    timeval tv = {};
    gettimeofday(&tv, nullptr);
    tm t = {};
    localtime_r((time_t *) &tv.tv_sec, &t);
    char buffer[EMX_LOG_MAX_SIZE + 1];
    int32_t len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]\033%s[%s]\033[0m[%s",
                          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
                          t.tm_sec, (uint32_t) tv.tv_usec / 1000,
                          LevelColor[(uint8_t) level], LevelName[(uint8_t) level], s);
    do { len--; } while (len && buffer[len] != '.');
    len += sprintf(buffer + len, ":%s:%d]", func, line);
    va_start(args, fmt);
    len += vsnprintf(buffer + len, EMX_LOG_MAX_SIZE - len, fmt, args);
    va_end(args);
    len = len >= EMX_LOG_MAX_SIZE ? EMX_LOG_MAX_SIZE : len;
    if (!m_shared || !m_shared->async)
        printf("%s", buffer);
    int flags = level > LevelE::Err ? MSG_DONTWAIT : 0;
    sendto(m_normal.sock, buffer, len + 1, flags, (sockaddr *) &m_normal.address, sizeof(m_normal.address));
}

void Log::Print(const char *file, const char *func, int32_t line, const char *fmt, ...) {
    va_list args;
    const char *s = strrchr(file, '/');
    s == nullptr ? s = file : s += 1;
    timeval tv = {};
    gettimeofday(&tv, nullptr);
    tm t = {};
    localtime_r((time_t *) &tv.tv_sec, &t);
    char buffer[EMX_LOG_MAX_SIZE + 1];
    int32_t len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%03d][%s",
                          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
                          t.tm_sec, (uint32_t) tv.tv_usec / 1000, s);
    do { len--; } while (len && buffer[len] != '.');
    len += sprintf(buffer + len, ":%s:%d]", func, line);
    va_start(args, fmt);
    len += vsnprintf(buffer + len, EMX_LOG_MAX_SIZE - len, fmt, args);
    va_end(args);
    len = len >= EMX_LOG_MAX_SIZE ? EMX_LOG_MAX_SIZE : len;
    printf("%s", buffer);
}

void Log::Print(const char *fmt, ...) {
    va_list args;
    timeval tv = {};
    gettimeofday(&tv, nullptr);
    tm t = {};
    localtime_r((time_t *) &tv.tv_sec, &t);
    char buffer[EMX_LOG_MAX_SIZE + 1];
    int32_t len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
                          t.tm_sec, (uint32_t) tv.tv_usec / 1000);
    va_start(args, fmt);
    len += vsnprintf(buffer + len, EMX_LOG_MAX_SIZE - len, fmt, args);
    va_end(args);
    len = len >= EMX_LOG_MAX_SIZE ? EMX_LOG_MAX_SIZE : len;
    if (!m_shared || !m_shared->async)
        printf("%s", buffer);
    int flags = MSG_DONTWAIT;
    sendto(m_normal.sock, buffer, len + 1, flags, (sockaddr *) &m_normal.address, sizeof(m_normal.address));
}

void Log::Save(const char *fmt, ...) {
    va_list args;
    timeval tv = {};
    gettimeofday(&tv, nullptr);
    tm t = {};
    localtime_r((time_t *) &tv.tv_sec, &t);
    char buffer[EMX_LOG_MAX_SIZE + 1];
    int32_t len = sprintf(buffer, "[%04d-%02d-%02d %02d:%02d:%02d]",
                          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    va_start(args, fmt);
    len += vsnprintf(buffer + len, EMX_LOG_MAX_SIZE - len, fmt, args);
    va_end(args);
    len = len >= EMX_LOG_MAX_SIZE ? EMX_LOG_MAX_SIZE : len;
    if (!m_shared || !m_shared->async)
        printf("%s", buffer);
    sendto(m_flash.sock, buffer, len + 1, 0, (sockaddr *) &m_flash.address, sizeof(m_flash.address));
}

void Log::SetLevel(LevelE level) {
    if (m_shared) {
        m_shared->level = level;
    }
}

void Log::SetAsync(bool ena) {
    if (m_shared) {
        m_shared->async = ena;
    }
}

ErrCodeE Log::SetConfig(Config &cfg) {
    ErrCodeE e;
    uint8_t buffer[sizeof(LogInner::CmdE) + sizeof(Log::Config)];
    buffer[0] = (uint8_t) LogInner::CmdE::ConfigSet;
    memcpy(buffer + 1, &cfg, sizeof(Log::Config));
    sendto(m_ctrl.sock, buffer, sizeof(buffer), 0, (sockaddr *) &m_ctrl.address, sizeof(m_ctrl.address));
    sockaddr_un addr = {};
    socklen_t len = sizeof(addr);
    int n = recvfrom(m_ctrl.sock, buffer, sizeof(buffer), 0, (sockaddr *) &addr, &len);
    if (n <= 0) {
        printf("recv from log server failed\n");
        return ErrCodeE::Failure;
    }
    if ((LogInner::CmdE) buffer[0] != LogInner::CmdE::ConfigSet) {
        printf("error cmd type %d\n", buffer[0]);
        return ErrCodeE::Failure;
    }
    memcpy(&e, buffer + 1, sizeof(ErrCodeE));
    return e;
}

ErrCodeE Log::GetConfig(Log::Config &cfg) {
    ErrCodeE e;
    LogInner::CmdE cmd = LogInner::CmdE::ConfigGet;
    sendto(m_ctrl.sock, &cmd, sizeof(cmd), 0, (sockaddr *) &m_ctrl.address, sizeof(m_ctrl.address));
    sockaddr_un addr = {};
    socklen_t len = sizeof(addr);
    uint8_t buffer[sizeof(LogInner::CmdE) + sizeof(ErrCodeE) + sizeof(Log::Config)];
    size_t n = recvfrom(m_ctrl.sock, buffer, sizeof(buffer), 0, (sockaddr *) &addr, &len);
    if (n < sizeof(buffer)) {
        printf("recv from log server failed\n");
        return ErrCodeE::Failure;
    }
    int off = 0;
    memcpy(&cmd, buffer + off, sizeof(cmd));
    off += sizeof(cmd);
    if (cmd != LogInner::CmdE::ConfigGet) {
        printf("error cmd type %d\n", (int) cmd);
        return ErrCodeE::Failure;
    }
    memcpy(&e, buffer + off, sizeof(e));
    off += sizeof(e);
    if (e != ErrCodeE::Success) {
        return e;
    }
    memcpy(&cfg, buffer + off, sizeof(cfg));
    return ErrCodeE::Success;
}

Log::LevelE Log::GetLevel() {
    if (m_shared) {
        return m_shared->level;
    } else {
        return LevelE::Debug;
    }
}

bool Log::GetAsync() {
    if (m_shared) {
        return m_shared->async;
    } else {
        return false;
    }
}

ErrCodeE Log::Pause() {
    ErrCodeE e;
    LogInner::CmdE cmd = LogInner::CmdE::Stop;
    sendto(m_ctrl.sock, &cmd, sizeof(cmd), 0, (sockaddr *) &m_ctrl.address, sizeof(m_ctrl.address));
    sockaddr_un addr = {};
    socklen_t len = sizeof(addr);
    uint8_t buffer[sizeof(LogInner::CmdE) + sizeof(ErrCodeE)];
    size_t n = recvfrom(m_ctrl.sock, buffer, sizeof(buffer), 0, (sockaddr *) &addr, &len);
    if (n < sizeof(buffer)) {
        printf("recv from log server failed\n");
        return ErrCodeE::Failure;
    }
    int off = 0;
    memcpy(&cmd, buffer + off, sizeof(cmd));
    off += sizeof(cmd);
    if (cmd != LogInner::CmdE::Stop) {
        printf("error cmd type %d\n", (int) cmd);
        return ErrCodeE::Failure;
    }
    memcpy(&e, buffer + off, sizeof(e));
    if (e != ErrCodeE::Success) {
        return e;
    }
    return ErrCodeE::Success;
}

ErrCodeE Log::Resume() {
    ErrCodeE e;
    LogInner::CmdE cmd = LogInner::CmdE::Start;
    sendto(m_ctrl.sock, &cmd, sizeof(cmd), 0, (sockaddr *) &m_ctrl.address, sizeof(m_ctrl.address));
    sockaddr_un addr = {};
    socklen_t len = sizeof(addr);
    uint8_t buffer[sizeof(LogInner::CmdE) + sizeof(ErrCodeE)];
    size_t n = recvfrom(m_ctrl.sock, buffer, sizeof(buffer), 0, (sockaddr *) &addr, &len);
    if (n < sizeof(buffer)) {
        printf("recv from log server failed\n");
        return ErrCodeE::Failure;
    }
    int off = 0;
    memcpy(&cmd, buffer + off, sizeof(cmd));
    off += sizeof(cmd);
    if (cmd != LogInner::CmdE::Start) {
        printf("error cmd type %d\n", (int) cmd);
        return ErrCodeE::Failure;
    }
    memcpy(&e, buffer + off, sizeof(e));
    if (e != ErrCodeE::Success) {
        return e;
    }
    return ErrCodeE::Success;
}

