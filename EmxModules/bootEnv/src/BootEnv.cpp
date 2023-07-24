//
// Created by xiong on 2022/2/27.
//
#include <sys/file.h>
#include "BootEnv.hpp"
#include "fw_env/fw_env.h"

using namespace Emx;

char g_bootEnvCfgDir[EMX_MAX_PATH_SIZE] = {};

void BootEnv::CheckCfgDir() {
    if (g_bootEnvCfgDir[0] != 0)
        return;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/boot", Misc::GetAppConfigsDir());
    if (!File::IsDir(path)) {
        emxloge("%s not exist\n", path);
        return;
    }
    strncpy(g_bootEnvCfgDir, path, sizeof(g_bootEnvCfgDir));
}

ErrCodeE BootEnv::GetEnv(const char *key, std::string &value) {
    char *env = nullptr;
    int maxSize = 0;
    int fd;
    ErrCodeE e = Lock(fd);

    if (e != ErrCodeE::Success)
        return e;
    CheckCfgDir();
    int ret = get_env(&env, &maxSize);
    UnLock(fd);
    if (ret) {
        emxloge("get env failed\n");
        return ErrCodeE::Failure;
    }

    char *p, *nxt;
    for (p = env; *p; p = nxt + 1) {
        for (nxt = p; *nxt; ++nxt) {
            if (nxt >= &env[maxSize]) {
                emxloge("environment not terminated\n");
                free(env);
                return ErrCodeE::Failure;
            }
        }
        char *equal = strstr(p, "=");
        if (equal) {
            *equal = '\0';
            if (strcmp(p, key) == 0) {
                value = equal + 1;
                break;
            }
        }
    }
    free(env);
    return ErrCodeE::Success;
}

ErrCodeE BootEnv::GetEnv(BootEnv::EnvMap &map) {
    char *env = nullptr;
    int maxSize = 0;
    int fd;

    ErrCodeE e = Lock(fd);
    if (e != ErrCodeE::Success)
        return e;
    CheckCfgDir();

    int ret = get_env(&env, &maxSize);
    UnLock(fd);
    if (ret) {
        emxloge("get env failed\n");
        return ErrCodeE::Failure;
    }

    char *p, *nxt;
    for (p = env; *p; p = nxt + 1) {
        for (nxt = p; *nxt; ++nxt) {
            if (nxt >= &env[maxSize]) {
                emxloge("environment not terminated\n");
                free(env);
                return ErrCodeE::Failure;
            }
        }
        char *equal = strstr(p, "=");
        if (equal) {
            *equal = '\0';
            map[p] = equal + 1;
        }
    }
    free(env);
    return ErrCodeE::Success;
}

ErrCodeE BootEnv::SetEnv(const char *key, const char *value) {
    ErrCodeE e;
    int fd;
    e = Lock(fd);
    if (e != ErrCodeE::Success)
        return e;
    CheckCfgDir();

    auto k = std::string(key);
    auto v = std::string(value);

    if (fw_env_open(nullptr)) {
        emxloge("env open failed\n");
        e = ErrCodeE::OpenFailed;
        goto UnLock;
    }

    if (fw_env_write((char *) k.c_str(), (char *) v.c_str())) {
        emxloge("env %s write failed\n", key);
        e = ErrCodeE::Failure;
        goto Close;
    }

    if (fw_env_flush(nullptr)) {
        emxloge("env flush failed\n");
        e = ErrCodeE::Failure;
        goto Close;
    }
    Close:
    fw_env_close(nullptr);
    UnLock:
    UnLock(fd);
    return e;
}

ErrCodeE BootEnv::SetEnv(BootEnv::EnvMap &map) {
    ErrCodeE e;
    int fd;
    e = Lock(fd);
    if (e != ErrCodeE::Success)
        return e;
    CheckCfgDir();

    if (fw_env_open(nullptr)) {
        emxloge("env open failed\n");
        e = ErrCodeE::OpenFailed;
        goto UnLock;
    }
    for (auto &item : map) {
        if (fw_env_write((char *) item.first.c_str(), (char *) item.second.c_str())) {
            emxloge("env %s write failed\n", item.first.c_str());
            e = ErrCodeE::Failure;
            goto Close;
        }
    }

    if (fw_env_flush(nullptr)) {
        emxloge("env flush failed\n");
        e = ErrCodeE::Failure;
        goto Close;
    }
    Close:
    fw_env_close(nullptr);
    UnLock:
    UnLock(fd);
    return e;
}


ErrCodeE BootEnv::Lock(int &fd) {
    const char *lockName = "/var/lock/BootEnv.lock";

    fd = open(lockName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (-1 == fd) {
        emxloge("Error opening lock file %s\n", lockName);
        return ErrCodeE::OpenFailed;
    }

    if (-1 == flock(fd, LOCK_EX)) {
        emxloge("Error locking file %s\n", lockName);
        close(fd);
        return ErrCodeE::ResInUsing;
    }
    return ErrCodeE::Success;
}

void BootEnv::UnLock(int &fd) {
    flock(fd, LOCK_UN);
    close(fd);
}


