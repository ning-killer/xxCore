//
// Created by xiong on 2022/6/20.
//
#include "EapilHangYanVersion.h"
#include "EmxCore.hpp"
#include "Ovd.hpp"

using namespace Emx;
Ovd *ovd = nullptr;

#if TestDef == 1
#endif

int main(int argc, char *argv[]) {
    if (!Misc::ApplicationExist("EmxCoreServer")) {
        emxloge("not found EmxCoreServer\n");
        return -1;
    }
    emxlogi("EAPIL_HANGYAN_VERSION=%s\n", EAPIL_HANGYAN_VERSION);
    Misc::KillBeforeRun(argv[0]);
    signal(SIGPIPE, SIG_IGN);
    // 第一个参数为-b代表后台执行
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        pid_t pid = fork();
        if (pid < 0) {
            printf("fork failed");
            exit(-1);
        } else if (pid != 0) {
            return 0;
        }
    }

    ovd = new Ovd();
    ovd->Create();
    while (true) {
        sleep(1);
    }
    ovd->Destroy();
    delete ovd;
    return 0;
}