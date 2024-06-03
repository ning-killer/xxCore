#include "EmxCore.hpp"
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace Emx;

int main(int argc, char *argv[]) {
    Param param_adec("media/normal3M/adec");
    Json::Value json;
    if (param_adec.Get(json) != ErrCodeE::Success) {
        emxloge("load adec failed\n");
        return -1;
    }
    emxlogd("json: %s\n", json.toStyledString().c_str());
}