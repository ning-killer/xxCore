/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include "EmxCore.hpp"

using namespace Emx;

static void usage() {
    printf("usage:DevTools log level [0-5] //0-crt,1-err,2-warn,3-info,4-dbg,5-trace, null means print cur level\n");
    printf("usage:DevTools log async [0,1] //0-sync,1-async, null means print cur async mode\n");
}

void CmdLog(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return;
    }
    auto level = emxLog.GetLevel();
    auto async = emxLog.GetAsync();

    if (strcmp(argv[1], "level") == 0) {
        if (argc == 2)
            printf("%d\n", (int) level);
        else {
            auto levelW = (Log::LevelE) atoi(argv[2]);
            emxLog.SetLevel(levelW);
            printf("set log level from %d to %d\n", (int) level, (int) levelW);
        }
    } else if (strcmp(argv[1], "async") == 0) {
        if (argc == 2)
            printf("%d\n", (int) async);
        else {
            auto asyncW = (bool) atoi(argv[2]);
            emxLog.SetAsync(asyncW);
            printf("set log async from %d to %d\n", (int) async, (int) asyncW);
        }
    } else {
        usage();
    }
}