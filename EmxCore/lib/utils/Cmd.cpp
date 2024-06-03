//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Cmd.hpp"
#include "core/EmxLog.hpp"
#include <cstdarg>
#include <error.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

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

ErrCodeE Cmd::VforkRun(const char *cmd, ...) {
    if (cmd == nullptr) {
        emxloge("cmd is null\n");
        return ErrCodeE::Failure;
    }

    // 将可变参数格式化为命令字符串
    va_list args;
    va_start(args, cmd);
    char formatCmd[1024] = { 0 };
    vsnprintf(formatCmd, sizeof(formatCmd), cmd, args);
    va_end(args); 

    pid_t pid = vfork();
    int status = -1;
    if (pid == -1) {
        emxloge("vfork failed: %s\n", formatCmd);
        return ErrCodeE::Failure;
    } else if (pid == 0) {  // 子进程
        for (int i = 3; i < 4096; i++){
            close(i);
        }
        execl("/bin/sh", "sh", "-c", formatCmd, nullptr);
		_exit(127); 
    } else { // 父进程
        while ((pid = waitpid(pid, &status, 0)) == -1) {
			if (errno != EINTR) {
				status = -1; 
				break;
			}
		}
    }
    return status == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
}