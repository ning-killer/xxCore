//
// Created by xiong on 2021/4/30.
//
#include "EmxCore.hpp"
#include "MediaConfigs.hpp"

using namespace Emx;


MediaConfigs::MediaConfigs(const char *dir) {
    strncpy(m_dir, dir, sizeof(m_dir));
    memset(m_scene, 0, sizeof(m_scene));
}

ErrCodeE MediaConfigs::SetScene(const char *scene) {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/%s", m_dir, scene);
    if (!File::Exist(path)) {
        emxloge("scene %s not exist", path);
        return ErrCodeE::ResNotExist;
    }
    strncpy(m_scene, scene, sizeof(m_scene));
    return ErrCodeE::Success;
}

bool MediaConfigs::CheckRangeLegal(const char *name, int param, Json::Value &json) {
    int min = json[name]["min"].asInt();
    int max = json[name]["max"].asInt();
    if (param < min || param > max) {
        emxloge("param %s %d invalid, range[%d,%d]\n", name, param, min, max);
        return false;
    }
    return true;
}

bool MediaConfigs::CheckItemLegal(const char *name, int param, Json::Value &json) {
    bool find = false;
    for (auto &e: json[name]) {
        if (e.asInt() == param) {
            find = true;
            break;
        }
    }
    if (!find) {
        emxloge("param %s %d invalid\n", name, param);
    }
    return find;
}

ErrCodeE MediaConfigs::GetCfg(const char *name, Json::Value &json) {
    char path[EMX_MAX_PATH_SIZE];
    snprintf(path, sizeof(path), "%s/%s/%s.json", m_dir, m_scene, name);
    auto e = EasyJson::Load(path, json);
    if (e != ErrCodeE::Success)
        emxloge("get media %s configuration failed\n", path);
    return e;
}

ErrCodeE MediaConfigs::GetRange(const char *name, Json::Value &json) {
    char path[EMX_MAX_PATH_SIZE];
    snprintf(path, sizeof(path), "%s/range/%s.json", m_dir, name);
    auto e = EasyJson::Load(path, json);
    if (e != ErrCodeE::Success)
        emxloge("get media %s range failed\n", path);
    return e;
}

ErrCodeE MediaConfigs::GetParam(const char *name, Json::Value &json) {
    char sceneName[EMX_MAX_PATH_SIZE];
    snprintf(sceneName, sizeof(sceneName), "media/%s/%s", m_scene, name);
    Param param(sceneName);
    return param.Get(json);
}

ErrCodeE MediaConfigs::SetParam(const char *name, Json::Value &json) {
    char sceneName[EMX_MAX_PATH_SIZE];
    snprintf(sceneName, sizeof(sceneName), "media/%s/%s", m_scene, name);
    Param param(sceneName);
    return param.Set(json);
}


