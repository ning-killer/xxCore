/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include "EmxCore.hpp"

extern void CmdParam(int argc, char *argv[]);

extern void CmdLog(int argc, char *argv[]);

extern void CmdConsoleRemap(int argc, char *argv[]);

extern void CmdMac(int argc, char *argv[]);

extern void CmdZone(int argc, char *argv[]);

using namespace Emx;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxlogx("illegal argument\n");
        return 0;
    }
    auto cmd = argv[1];
    argc--;
    argv++;
    if (strcmp(cmd, "param") == 0) {
        CmdParam(argc, argv);
    } else if (strcmp(cmd, "log") == 0) {
        CmdLog(argc, argv);
    } else if (strcmp(cmd, "consoleRemap") == 0) {
        CmdConsoleRemap(argc, argv);
    }else if (strcmp(cmd, "setMac") == 0) {
        CmdMac(argc, argv);
    }else if (strcmp(cmd, "setZone") == 0){
        CmdZone(argc, argv);
    }
    else {
        printf("unsupported cmd %s\n", cmd);
    }
    return 0;
}