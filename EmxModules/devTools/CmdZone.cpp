#include <core/utils/Misc.hpp>
#include <core/utils/File.hpp>
#include <core/utils/Crypto.hpp>
#include "core/utils/Cmd.hpp"
#include "json/json.h"
#include <stdio.h>

static uint32_t teaCipher[32] = {
        0xf8729d84, 0x7999d514, 0xab3127cb, 0x3e60561e, 0x9cb50ac6, 0xd62488df, 0x4bb57760, 0x8cd144aa,
        0xf9af5fc1, 0x537634ee, 0x7a75da64, 0x1d0480a0, 0xd24bfc1e, 0x7ee597a6, 0x81535bd4, 0x958417ad,
        0xdaa521e8, 0x0a34bdad, 0x4c1a0398, 0xcddba8d0, 0xcd484810, 0x4470bc80, 0x8f082387, 0x1ec8cd06,
        0xbeadb791, 0xc1898a02, 0x1f92c69c, 0x3f3caf16, 0x03fc7a19, 0x3a0fbd11, 0x9d7edf74, 0x7cbf8afb};
using namespace Emx;
void CmdZone(int argc, char *argv[])
{
    char *appConfigs =getenv("EMX_APP_CONFIGS_DIR");
    char pathConfigs[128] = {0};
    std::string coreStr;
    sprintf(pathConfigs,"%s/core.json",appConfigs);

    ErrCodeE err = File::Read(pathConfigs,coreStr);
    if (err!=ErrCodeE::Success)
    {
        printf("read file error!\n");
        return;
    }

    Json::Value coreJson,zoneJson;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());
    if (!reader->parse(coreStr.data(),coreStr.data()+coreStr.size(), &coreJson, &errs)) {
        printf("%s Cannot parse : %s\n", coreStr.data(),
                errs.empty() ? "unknown" : errs.data());
    }

    memset(pathConfigs,0,sizeof (pathConfigs));
    sprintf(pathConfigs,"%s/zone.json",coreJson["param"]["normal"].asCString());
    if(!File::Exist(pathConfigs)){
        memset(pathConfigs,0,sizeof (pathConfigs));
        sprintf(pathConfigs,"%s/zone.json",coreJson["param"]["default"].asCString());
    }

    std::string zoneStr;
    err = File::Read(pathConfigs,zoneStr);
    if (err!=ErrCodeE::Success)
    {
        printf("read file error!\n");
        return;
    }

    TEA tea;
    uint8_t key[17];
    uint8_t iv[17];

    tea.Decrypt(teaCipher, key, iv);
    AES_CBC aes;
    aes.Init(key, iv);
    aes.Decrypt((uint8_t *) zoneStr.data(), zoneStr.size());
    Pkcs7Padding::UnPad(zoneStr);

    if (!reader->parse(zoneStr.data(),zoneStr.data()+ zoneStr.size(), &zoneJson, &errs)) {
        printf("%s Cannot parse : %s\n", zoneStr.data(),
               errs.empty() ? "unknown" : errs.data());
    }

    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, EMX_MAX_PATH_SIZE, "%s/zoneinfo/%s", Misc::GetAppConfigsDir(), zoneJson["zone"].asCString());
    if (!File::Exist(path)) {
        printf("zone %s not exist\n", zoneJson["zone"].asCString());
        return ;
    }

    snprintf(path, EMX_MAX_PATH_SIZE, "%s/zoneinfo/%s", Misc::GetAppConfigsDir(), zoneJson["zone"].asCString());
    if (Cmd::RunCheck("cp %s /tmp/localtime", path) != ErrCodeE::Success) {
        printf("cp %s /tmp/localtime failed\n", path);
        return ;
    }

    return;
}