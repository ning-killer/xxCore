#pragma once
#include <string>
#include <vector>

class DeviceConfig {
public:
    ~DeviceConfig(){}

    static DeviceConfig& Instance();

    int Load(std::string cfgFile);

public:
    std::string ServerIP;
    int ServerPort = 5060;
    std::string ServerID;
    std::string ServerDomain;
    std::string Password;

    std::string DeviceID;
    int Expires = 3600;
    int HeartbeatInterval = 60;
    int HeartbeatCount = 3;
    std::vector<std::string> Channels;
    
    int Port = 5060;
   
private:
    DeviceConfig() = default;
};