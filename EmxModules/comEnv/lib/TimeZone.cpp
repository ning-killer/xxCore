/*
* @Author: xiong
* @Date: 2022/9/23
*/

#include "TimeZone.hpp"

using namespace Emx;

ErrCodeE TimeZone::Load() {
    Param param("zone");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }
    strncpy(m_zone, json["zone"].asCString(), sizeof(m_zone));
    return ErrCodeE::Success;
}

ErrCodeE TimeZone::CopyTimeZoneToLocalTime() {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, EMX_MAX_PATH_SIZE, "%s/zoneinfo/%s", Misc::GetAppConfigsDir(), m_zone);
    if (Cmd::RunCheck("cp %s /tmp/localtime", path) != ErrCodeE::Success) {
        emxloge("cp %s /tmp/localtime failed\n", path);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE TimeZone::Set(const char *zone) {
    int size = (int) strlen(zone);
    if (size >= (int) sizeof(m_zone)) {
        emxloge("%s size %d > max size %d\n", zone, size, sizeof(m_zone));
        return ErrCodeE::IllegalParam;
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, EMX_MAX_PATH_SIZE, "%s/zoneinfo/%s", Misc::GetAppConfigsDir(), zone);
    if (!File::Exist(path)) {
        emxloge("zone %s not exist\n", zone);
        return ErrCodeE::ResNotExist;
    }
    strncpy(m_zone, zone, sizeof(m_zone));
    CopyTimeZoneToLocalTime();
    Param param("zone");
    Json::Value json;
    json["zone"] = m_zone;
    return param.Set(json);
}

void TimeZone::GetZoneList(std::vector<std::string> &list) {

    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, EMX_MAX_PATH_SIZE, "%s/zoneinfo", Misc::GetAppConfigsDir());
    list.clear();
    GetZoneListDir(path, "", list);
}

void TimeZone::GetZoneListDir(const char *path, const std::string &name, std::vector<std::string> &list) {
    DIR *dir = opendir(path);
    if (!dir) {
        emxloge("cannot open directory: %s\n ", path);
        return;
    }
    //top dir scan date
    struct dirent *dateEntry;
    while ((dateEntry = readdir(dir)) != nullptr) {
        if (dateEntry->d_type != DT_DIR) {
            if (name.empty())
                list.emplace_back(dateEntry->d_name);
            else
                list.emplace_back(name + "/" + dateEntry->d_name);
            continue;
        }
        if (strcmp(dateEntry->d_name, ".") == 0 || strcmp(dateEntry->d_name, "..") == 0)
            continue;
        char datePath[EMX_MAX_PATH_SIZE] = {};
        snprintf(datePath, sizeof(datePath), "%s/%s", path, dateEntry->d_name);
        GetZoneListDir(datePath, dateEntry->d_name, list);
    }
    closedir(dir);
}

