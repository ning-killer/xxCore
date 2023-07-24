//
// Created by cx on 2023/3/13.
//

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include "core/EmxParam.hpp"
#include "core/utils/Cmd.hpp"
#include "json/json.h"

static void usage() {
    printf("usage:DevTools cmdmac\n");
}


void set_mac(char *mac) {

    Json::Value netJson;
    Emx::Param param("netServer");
    param.Get(netJson);

    for (auto &json: netJson["dev"]) {
        if (json["name"].asString() == netJson["main"].asString()) {
            Emx::Cmd::Run("ifconfig %s down", json["interface"].asCString());
            Emx::Cmd::Run("ifconfig %s hw ether %s", json["interface"].asCString(), mac);
            Emx::Cmd::Run("ifconfig %s up", json["interface"].asCString());
        }
    }
}

void CmdMac(int argc, char *argv[]) {
    if (argc < 1) {
        usage();
        return;
    }
    int sock;

    struct ifreq ifreq;
    Json::Value macJson;
    Emx::Param param("mac");
    param.Get(macJson);
    if (0 == macJson["mac"].asString().size()) {
        char mac[128] = {0};
        Json::Value tmpMacJson;
        Emx::Param paramTmp("tmpMac");
        paramTmp.Get(tmpMacJson);
        if (0 == tmpMacJson["temMac"].asString().size()) {
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("socket");
                return;
            }
            strcpy(ifreq.ifr_name, "eth0");
            if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
                perror("ioctl");
                return;
            }
            sprintf(mac, "%02x%02x%02x%02x%02x%02x", (unsigned char) ifreq.ifr_hwaddr.sa_data[0],
                    (unsigned char) ifreq.ifr_hwaddr.sa_data[1],
                    (unsigned char) ifreq.ifr_hwaddr.sa_data[2],
                    (unsigned char) ifreq.ifr_hwaddr.sa_data[3],
                    (unsigned char) ifreq.ifr_hwaddr.sa_data[4],
                    (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
            printf("temporary mac :%s\n", mac);
            tmpMacJson["temMac"] = mac;
            paramTmp.Set(tmpMacJson);
        } else {
            set_mac((char *) tmpMacJson["temMac"].asCString());
        }
    } else {
        set_mac((char *) macJson["mac"].asCString());
    }

}