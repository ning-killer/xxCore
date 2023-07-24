//
// Created by xiong on 2021/7/20.
//

#include <csignal>
#include "EmxCore.hpp"
#include "EmxCoreImpVersion.h"
#include "Core.hpp"

using namespace Emx;
static Sem *gSem = nullptr;

static void StartUpDone() {
    if (gSem) {
        gSem->Post();
        gSem->Destroy();
        delete gSem;
        gSem = nullptr;
    }
}

int main(int argc, char *argv[]) {
    printf("EmxCoreServer version = [%s]\n", EMX_CORE_IMP_VERSION);
    Misc::KillBeforeRun(argv[0]);
    signal(SIGPIPE, SIG_IGN);
    // 第一个参数为-b代表后台执行
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        gSem = new Sem;
        gSem->Create("/CoreServerSem", 0);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        pid_t pid = fork();
        if (pid < 0) {
            printf("fork failed");
            exit(-1);
        } else if (pid != 0) {
            gSem->Wait(5000);
            gSem->Destroy();
            delete gSem;
            return 0;
        }
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/core.json", Misc::GetAppConfigsDir());
    Core core;
    core.Create(path);
    StartUpDone();
    emxlogi("EmxCoreServer version = [%s]\n", EMX_CORE_IMP_VERSION);
    while (true)sleep(1);
    sleep(20);
    core.Destroy();
}