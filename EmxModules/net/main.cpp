/*
* @Author: xiong
* @Date: 2023/5/31
*/
#include "csignal"
#include "EmxCore.hpp"
#include "NetServerLib.hpp"

using namespace Emx;
static Sem *gSem = nullptr;

void StartUpDone() {
    if (gSem) {
        gSem->Post();
        gSem->Destroy();
        delete gSem;
        gSem = nullptr;
    }
}

int main(int argc, char *argv[]) {
    if (!Misc::ApplicationExist("EmxCoreServer")) {
        emxloge("not found EmxCoreServer\n");
    }
    Misc::KillBeforeRun(argv[0]);
    signal(SIGPIPE, SIG_IGN);
    // 第一个参数为-b代表后台执行
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        gSem = new Sem;
        gSem->Create("/NetServerSem", 0);
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
    NetServerLib serverLib;
    serverLib.Create(StartUpDone, false);
    return 0;
}
