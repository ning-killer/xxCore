/**
 * @file   DemoMac.cpp
 * @author hening
 * @brief  区域业务测试
 * @date   2023-09-21 补充mac的c实现测试获取
 */

#include "EmxCore.hpp"
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace Emx;

std::string GetMac() {
    std::string mac = "";
    FILE* pipe = popen("ifconfig", "r");
    if (!pipe) {
        return mac;
    }
    char buffer[256] = { 0 };
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    size_t pos = result.find("HWaddr");
    if (pos != std::string::npos) {
        mac = result.substr(pos + 6 + 1, 17);
    }
    return mac;
}

int main(int argc, char *argv[]) {
    int sock, if_count, i;
    struct ifconf ifc;
    struct ifreq ifr[10];
    unsigned char mac[6];
    memset(&ifc, 0, sizeof(struct ifconf));
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    ifc.ifc_len = 10 * sizeof(struct ifreq);
    ifc.ifc_buf = (char *)ifr;
    //获取所有网卡信息
    ioctl(sock, SIOCGIFCONF, (char *)&ifc);
    if_count = ifc.ifc_len / (sizeof(struct ifreq));
    emxlogd("if_count:[%d]\n", if_count);
    for (i = 0; i < if_count; i++) {        
        if (ioctl(sock, SIOCGIFHWADDR, &ifr[i]) == 0) {  
            memcpy(mac, ifr[i].ifr_hwaddr.sa_data, 6);
            printf("eth: %s, mac: %02x:%02x:%02x:%02x:%02x:%02x\n", ifr[i].ifr_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        } 
    }
    emxlogd("GetMac:%s\n", GetMac().c_str());
    return 0;
}