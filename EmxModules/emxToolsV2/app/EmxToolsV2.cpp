//
// Created by xiong on 2021/5/20.
//
#include "EmxToolsV2.hpp"
#include "BroadcastService.hpp"
#include "ConfigService.hpp"
#include "FileService.hpp"

using namespace Emx;



EmxToolsV2::EmxToolsV2() {
    m_broadcast = new BroadcastService;
    m_config = new ConfigService;
    m_file = new FileService;
}

EmxToolsV2::~EmxToolsV2() {
    delete m_broadcast;
    delete m_config;
    delete m_file;
}

void EmxToolsV2::Destroy() {

}

void EmxToolsV2::Create(bool newThread) {
    Json::Value json;
    auto path = new std::string(Misc::GetAppConfigsDir());
    path->append("/emxToolsV2.json");
    if (!File::Exist(path->c_str())) {
        printf("%s not exist\n", path->c_str());
        delete path;
        return;
    }
    if (EasyJson::Load(path->c_str(), json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path->c_str());
        delete path;
        return;
    }
    delete path;
    m_res.bufferSize = json["bufferSizeKB"].asInt() * 1024;
    m_res.buffer = (char *) malloc(m_res.bufferSize);
    if (!m_res.bufferSize) {
        emxloge("malloc %d failed\n", m_res.bufferSize);
        return;
    }
    m_res.loop.Init("EmxToolsV2", [this]() {
        m_broadcast->Destroy();
        m_config->DestroyConfigService();
        m_file->DestroyFileService();
    });
    m_broadcast->Create(&m_res, json["broadcast"]);
    m_config->CreateConfigService(&m_res, json["config"]);
    m_file->CreateFileService(&m_res, json["file"]);
    emxlogi("EmxToolsV2 Created\n");
    m_res.loop.Start(false);
}
