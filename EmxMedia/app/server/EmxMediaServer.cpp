//
// Created by xiong on 2021/7/30.
//
#include "EmxCore.hpp"
#include "MediaImpVersion.h"
#include "MediaServer.hpp"
#include "MediaFactory.hpp"
#include MEDIA_PROFILE_HEADER

using namespace Emx;
static Sem *gSem = nullptr;
MediaServer *gServer = nullptr;

static void StartUpDone() {
    if (gSem) {
        gSem->Post();
        gSem->Destroy();
        delete gSem;
        gSem = nullptr;
    }
}

int main(int argc, char *argv[]) {
    emxlogi("EmxMediaServer version = [%s]\n", EMX_MEDIA_IMP_VERSION);
    Misc::KillBeforeRun(argv[0]);
    signal(SIGPIPE, SIG_IGN);

    if (!Misc::ApplicationExist("EmxCoreServer")) {
        emxloge("not found EmxCoreServer\n");
        return -1;
    }

    // 第一个参数为-b代表后台执行
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        gSem = new Sem;
        gSem->Create("/MediaServerSem", 0);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        pid_t pid = fork();
        if (pid < 0) {
            printf("fork failed");
            exit(-1);
        } else if (pid != 0) {
            gSem->Wait(6000);
            gSem->Destroy();
            delete gSem;
            return 0;
        }
    }

    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s", Misc::GetMediaConfigsDir());
    if (!File::IsDir(path)) {
        printf("%s not exist\n", path);
        return -1;
    }

    MediaFactory *factory = new MEDIA_PROFILE_FACTORY;
    gServer = factory->MakeServer(path, factory);
    gServer->Create(StartUpDone);
    while (true)sleep(1);
    sleep(20);
    gServer->Destroy();
}
