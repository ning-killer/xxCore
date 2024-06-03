/**
 * @file   DemoLoop.cpp
 * @author hening
 * @brief  euvloop测试
 * @date   2023-10-20
 */

#include "EmxCore.hpp"
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace Emx;

EuvTimer g_timer;
EuvLoop g_loop;

#if 0
void Stop() {
    if (g_loop.IsRunning()) {
        g_loop.Stop();
    }
    g_timer.Stop();
    emxlogd("handle stop...\n");
}

void Start() {
    if (g_loop.IsRunning()) {
        g_loop.Stop();
    }
    g_loop.Start(true);
    g_timer.Stop();
    g_timer.Start(0, 500, []() {
        emxlogd("handle run...\n");        
    });
}
#endif

#if 1

void Start() {
    g_loop.StopAndDeInit();
    g_loop.Init("testloop", []() {  g_timer.Destroy();});
    g_timer.Create(g_loop);
    g_timer.Stop();
    g_timer.Start(0, 500, []() {
        emxlogd("handle run...\n");        
    });
    g_loop.Start(true);
}
#endif

int main(int argc, char *argv[]) {
    EuvLoop loop;
    loop.Init("DemoLoop", []() { });
    emxlogd("DemoLoop start.\n");
    g_loop.Init("testloop", []() { g_timer.Destroy(); });
    g_timer.Create(g_loop);
    g_loop.Start(true);
    for (int i = 0; i < 10; ++i) {
        Start();
        sleep(1);
    }
    loop.Start(false);
}