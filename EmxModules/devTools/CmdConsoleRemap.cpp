/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>

static void usage() {
    printf("usage:DevTools consoleRemap on[off] //on:remap console to current tty,off:resume console to origin\n");
}

void CmdConsoleRemap(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return;
    }
    /* 获取当前tty名称 */
    char *tty_name = ttyname(STDOUT_FILENO);
    printf("tty name: %s\n", tty_name);
    int tty = -1;
    if (strcmp(argv[1], "on") == 0) { /* 重定向console到当前tty */
        tty = open(tty_name, O_RDONLY | O_WRONLY);
        ioctl(tty, TIOCCONS);
        perror("ioctl TIOCCONS");
    } else if (strcmp(argv[1], "off") == 0) { /* 恢复console */
        tty = open("/dev/console", O_RDONLY | O_WRONLY);
        ioctl(tty, TIOCCONS);
        perror("ioctl TIOCCONS");
    } else {
        usage();
    }
    close(tty);
}