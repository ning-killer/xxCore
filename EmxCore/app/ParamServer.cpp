//
// Created by xiong on 2022/3/2.
//
#include <dirent.h>
#include <set>
#include <sys/stat.h>
#include "ParamServer.hpp"
#include "core/EmxLog.hpp"
#include "core/EmxUtils.hpp"
#include "ParamInner.hpp"
#include "CoreConfigs.hpp"

using namespace Emx;
static uint32_t teaCipher[32] = {
        0xf8729d84, 0x7999d514, 0xab3127cb, 0x3e60561e, 0x9cb50ac6, 0xd62488df, 0x4bb57760, 0x8cd144aa,
        0xf9af5fc1, 0x537634ee, 0x7a75da64, 0x1d0480a0, 0xd24bfc1e, 0x7ee597a6, 0x81535bd4, 0x958417ad,
        0xdaa521e8, 0x0a34bdad, 0x4c1a0398, 0xcddba8d0, 0xcd484810, 0x4470bc80, 0x8f082387, 0x1ec8cd06,
        0xbeadb791, 0xc1898a02, 0x1f92c69c, 0x3f3caf16, 0x03fc7a19, 0x3a0fbd11, 0x9d7edf74, 0x7cbf8afb};

ParamServer::ParamServer() : m_key(), m_iv() {
    auto &json = CoreConfigs::GetInst()->GetCfg()["param"];
    strncpy(m_normal, json["normal"].asCString(), sizeof(m_normal) - 1);
    strncpy(m_backup, json["backup"].asCString(), sizeof(m_backup) - 1);
    strncpy(m_default, json["default"].asCString(), sizeof(m_default) - 1);
    TEA tea;
    tea.Decrypt(teaCipher, m_key, m_iv);
}


void ParamServer::Create() {
    mkdir(m_normal, 0777);
    mkdir(m_backup, 0777);

    //创建normal和backup中default param中对应的子目录
    CreateSubDir(m_default, m_normal, m_backup);

    m_loop.Init("ParamServer", [this]() {
        m_rep.DestroyMsgRpcRep();
    });
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.clientId = PARAM_SERVER_MSG_ID;
    cfg.buffer = m_buffer;
    cfg.size = sizeof(m_buffer);
    cfg.OnRecvReq = std::bind(&ParamServer::OnRecvReq, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_rep.CreateMsgRpcRep(cfg);
    m_loop.Start();
}

void ParamServer::Destroy() {
    m_loop.StopAndDeInit();
}

ErrCodeE ParamServer::OnRecvReq(uint32_t module, uint32_t method,
                                const char *data, int32_t size, std::string &out) {
    emxlogt("[%#x][%u][%s]\n", module, method, size ? data : " ");
    if (PARAM_SERVER_MOD != module) {
        emxloge("not support module[%#x]\n", module);
        return ErrCodeE::OperationNotSupport;
    }
    const char *name = data;

    switch ((ParamServerMethodE) method) {
        case ParamServerMethodE::ResetToFactory:
            return ResetToFactory();
        case ParamServerMethodE::Set:
            return Set(name, data + strlen(data) + 1);
        case ParamServerMethodE::Get:
            return Get(name, out);
        case ParamServerMethodE::GetDefault:
            return GetDefault(name, out);
        case ParamServerMethodE::Reset:
            return Reset(name);
        default:
            emxloge("not support method[%d]\n", method);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE ParamServer::Set(const char *name, const char *data) {
    char def[EMX_MAX_PATH_SIZE] = {};
    char normal[EMX_MAX_PATH_SIZE] = {};
    char backup[EMX_MAX_PATH_SIZE] = {};
    snprintf(def, sizeof(def), "%s/%s.json", m_default, name);
    snprintf(normal, sizeof(normal), "%s/%s.json", m_normal, name);
    snprintf(backup, sizeof(backup), "%s/%s.json", m_backup, name);

    std::string saved;
    if (Load(normal, saved) == ErrCodeE::Success) {
        if (memcmp(saved.data(), data, saved.size()) == 0)
            return ErrCodeE::Success;
        else
            File::Copy(normal, backup);
    }
    std::string text = data;
#ifndef EMX_PLAIN_PARAM
        Pkcs7Padding::Pad(text);
        m_aes.Init(m_key, m_iv);
        m_aes.Encrypt((uint8_t *) text.data(), text.size());
        std::string base64;
        Base64::Encode(text, base64);
        text = base64;
#endif
    auto e = File::Write(normal, (char *) text.data(), (int) text.size());
    return e;
}

ErrCodeE ParamServer::Get(const char *name, std::string &data) {
    char def[EMX_MAX_PATH_SIZE] = {};
    char normal[EMX_MAX_PATH_SIZE] = {};
    char backup[EMX_MAX_PATH_SIZE] = {};
    snprintf(def, sizeof(def), "%s/%s.json", m_default, name);
    snprintf(normal, sizeof(normal), "%s/%s.json", m_normal, name);
    snprintf(backup, sizeof(backup), "%s/%s.json", m_backup, name);
    if (Load(normal, data) == ErrCodeE::Success)
        return ErrCodeE::Success;
    if (Load(backup, data) == ErrCodeE::Success) {
        File::Copy(backup, normal);
        emxloge("%s normal load failed, load backup\n", name);
        return ErrCodeE::Success;
    }
    auto e = Load(def, data);
    return e;
}

ErrCodeE ParamServer::GetDefault(const char *name, std::string &data) {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/%s.json", m_default, name);
    return Load(path, data);
}

ErrCodeE ParamServer::Reset(const char *name) {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/%s.json", m_backup, name);
    File::Remove(path);
    snprintf(path, sizeof(path), "%s/%s.json", m_normal, name);
    File::Remove(path);
    return ErrCodeE::Success;
}

ErrCodeE ParamServer::Load(const char *path, std::string &data) {
    data.clear();
    if (File::Read(path, data) != ErrCodeE::Success)
        return ErrCodeE::Failure;
#ifndef EMX_PLAIN_PARAM
        std::string tmp;
        Base64::Decode(data, tmp);
        m_aes.Init(m_key, m_iv);
        m_aes.Decrypt((uint8_t *) tmp.data(), tmp.size());
        Pkcs7Padding::UnPad(tmp);
        data = tmp;
#endif
    Json::Value dataJson;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());
    if (!reader->parse(data.data(), data.data() + data.size(), &dataJson, &errs)) {
        emxloge("%s Cannot parse : %s\n", data.data(),
                errs.empty() ? "unknown" : errs.data());
        return ErrCodeE::ParseFailed;
    }
    return ErrCodeE::Success;
}

void ParamServer::CreateSubDir(char *def, char *normal, char *backup) {
    DIR *pDef = opendir(def);
    if (!pDef)
        return;
    struct dirent *entry;
    while ((entry = readdir(pDef)) != nullptr) {
        if (entry->d_type & DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char defNewDir[EMX_MAX_PATH_SIZE] = {};
            char normalNewDir[EMX_MAX_PATH_SIZE] = {};
            char backupNewDir[EMX_MAX_PATH_SIZE] = {};
            snprintf(defNewDir, sizeof(defNewDir), "%s/%s", def, entry->d_name);
            snprintf(normalNewDir, sizeof(normalNewDir), "%s/%s", normal, entry->d_name);
            snprintf(backupNewDir, sizeof(backupNewDir), "%s/%s", backup, entry->d_name);
            mkdir(normalNewDir, 0777);
            mkdir(backupNewDir, 0777);
            CreateSubDir(defNewDir, normalNewDir, backupNewDir);
        }
    }
    closedir(pDef);
}

void ScanDir(const char *dir, const char *name,
             std::set<std::string> &reservedSet, std::list<std::string> &removeList) {
    DIR *pDir = opendir(dir);
    if (!pDir)
        return;
    struct dirent *entry;
    while ((entry = readdir(pDir)) != nullptr) {
        if (entry->d_type & DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            char subDir[EMX_MAX_PATH_SIZE] = {};
            snprintf(subDir, sizeof(subDir), "%s/%s", dir, entry->d_name);
            char subName[EMX_MAX_PATH_SIZE] = {};
            snprintf(subName, sizeof(subName), "%s%s/", name, entry->d_name);
            ScanDir(subDir, subName, reservedSet, removeList);
            continue;
        }
        char tmp[EMX_MAX_PATH_SIZE] = {};
        snprintf(tmp, sizeof(tmp), "%s%s", name, entry->d_name);
        if (reservedSet.find(tmp) == reservedSet.end()) {
            snprintf(tmp, sizeof(tmp), "%s/%s", dir, entry->d_name);
            removeList.emplace_back(tmp);
        } else {
            emxlogi("reserved %s\n", tmp);
        }
    }
    closedir(pDir);

}

ErrCodeE ParamServer::ResetToFactory() {
    std::string data;
    if (Get("resetReserved", data) != ErrCodeE::Success) {
        emxloge("get resetReserved.json failed\n");
        return ErrCodeE::Failure;
    }
    Json::Value reservedJson;
    if (EasyJson::Parse(data, reservedJson) != ErrCodeE::Success) {
        emxloge("parse resetReserved.json failed\n");
        return ErrCodeE::Failure;
    }
    std::set<std::string> reservedSet;
    for (auto &item: reservedJson)
        reservedSet.emplace(item.asString());

    std::list<std::string> removeList;
    ScanDir(m_backup, "", reservedSet, removeList);
    ScanDir(m_normal, "", reservedSet, removeList);
    for (auto &item: removeList) {
        File::Remove(item.c_str());
        emxlogi("remove %s\n", item.c_str());
    }
    return ErrCodeE::Success;
}
