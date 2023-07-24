#include "DeviceConfig.h"
#include "cJSON.h"
#include <memory>

DeviceConfig& DeviceConfig::Instance(){
    static std::shared_ptr<DeviceConfig> g_instance(new DeviceConfig);
    static DeviceConfig& g_instance_ref = *g_instance;
    return g_instance_ref;
}

int DeviceConfig::Load(std::string cfgFile){
    int ret = 1;
    FILE* fPtr = nullptr;
#ifdef _WIN32
    fopen_s(&fPtr, cfgFile.c_str(), "r");
#else
    fPtr = fopen(cfgFile.c_str(), "r");
#endif 
    if (!fPtr)
        return ret;
    fseek(fPtr, 0, SEEK_END);
    auto fileSize = ftell(fPtr);
    rewind(fPtr);
    char* data = new char[fileSize];
    fread(data, 1, fileSize, fPtr);
    fclose(fPtr);
    cJSON* root;

    if ((root = cJSON_Parse(data)))
    {
        ServerIP = cJSON_GetObjectItem(root, "server_ip")->valuestring;
        ServerPort = cJSON_GetObjectItem(root, "server_port")->valueint;
        ServerID = cJSON_GetObjectItem(root, "server_id")->valuestring;
        ServerDomain = cJSON_GetObjectItem(root, "server_domain")->valuestring;
        Password = cJSON_GetObjectItem(root, "password")->valuestring;

        auto chanel = cJSON_GetObjectItem(root, "channel_id");
        if (cJSON_IsArray(chanel))
        {
            auto sz = cJSON_GetArraySize(chanel);
            for (decltype(sz) i = 0; i < sz; i++)
            {
                auto item = cJSON_GetArrayItem(chanel, i);
                Channels.push_back(item->valuestring);
            }
        }
        DeviceID = cJSON_GetObjectItem(root, "device_id")->valuestring;
        Expires = cJSON_GetObjectItem(root, "reg_expires")->valueint;
        HeartbeatInterval = cJSON_GetObjectItem(root, "heartbeat_interval")->valueint;
        HeartbeatCount = cJSON_GetObjectItem(root, "heartbeat_count")->valueint;

        cJSON_Delete(root);
        ret = 0;
    }
    delete[] data;
    return ret;
}