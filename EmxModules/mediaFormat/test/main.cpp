/*
* @Author: xiong
* @Date: 2023/3/7
*/

#include "ReadMp4.hpp"
#include "WriteMp4.hpp"
#include "ReadFlv.hpp"
#include "WriteFlv.hpp"
#include "StreamFlv.hpp"
using namespace Emx;
#if 0
const char *path = "/mnt/nfs/testfmp4.mp4";

int main(int argc, char *argv[]) {
    if (*argv[1] == 'w') {
        WriteMp4 w;
        w.Create(path);
        sleep(20);
        w.Destroy();
    } else if (*argv[1] == 'r') {
        ReadMp4 r;
        r.Create(path);
    }
    return 0;
}
#endif

#if 1
const char *path = "/mnt/nfs/testflv.flv";

int main(int argc, char *argv[]) {
    if (*argv[1] == 'w') {
        WriteFlv w;
        w.Create(path);
        sleep(20);
        w.Destroy();
    } else if (*argv[1] == 'r') {
//        ReadFlv r;
//        r.Create(path);
    } else if (*argv[1] == 's') {
        StreamFlv s;
        s.Create();
        sleep(20);
        s.Destroy();
//        ReadFlv r;
//        r.Create(path);
    }
    return 0;
}

#endif