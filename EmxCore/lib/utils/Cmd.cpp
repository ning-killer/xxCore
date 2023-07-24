//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Cmd.hpp"
#include "core/EmxLog.hpp"
#include <cstdarg>

using namespace Emx;


ErrCodeE Cmd::Run(const char *cmd, ...) {
    va_list args;
    char temp[1024];
    FILE *pp;
    va_start(args, cmd);
    vsnprintf(temp, 1024, cmd, args);
    va_end(args);
    emxlogt("#%s#\n", temp);
    pp = popen(temp, "r");
    if (pp) {
        while (fgets(temp, 1024, pp)) {
            //xlog(XLOG_INFO, MOD_XCMD, temp);
        }
        pclose(pp);
        return ErrCodeE::Success;
    }
    emxloge(temp);
    return ErrCodeE::Failure;
}

ErrCodeE Cmd::RunCheck(const char *cmd, ...) {
    va_list args;
    char temp[1024];
    FILE *pp;
    const char *check = "RunCheckDone";

    va_start(args, cmd);
    int n = vsnprintf(temp, 1024, cmd, args);
    va_end(args);
    snprintf(temp + n, 1024 - n, "&&echo %s", check);
    emxlogt("#%s#\n", temp);
    pp = popen(temp, "r");
    if (pp) {
        while (fgets(temp, 1024, pp)) {
            if (strstr(temp, check)) {
//                xlog(XLOG_INFO, MOD_XCMD, "get:%s\n", check);
                pclose(pp);
                return ErrCodeE::Success;
            }
        }
        pclose(pp);
    }
    emxloge("%s\n", temp);
    return ErrCodeE::Failure;
}

ErrCodeE Cmd::System(const char *cmd, ...) {
    va_list args;
    char temp[1024];
    va_start(args, cmd);
    vsnprintf(temp, 1024, cmd, args);
    va_end(args);
    emxlogd("#%s#\n", temp);
    system(temp);
    return ErrCodeE::Failure;
}
