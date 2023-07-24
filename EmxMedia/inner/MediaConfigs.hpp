//
// Created by xiong on 2021/4/23.
//

#ifndef EMX_MediaServerConfigs_HPP
#define EMX_MediaServerConfigs_HPP

#include "json/json.h"
#include "core/EmxTypeDef.hpp"

namespace Emx {

    class MediaConfigs {
    public:
        MediaConfigs(const char *dir);

        const char *GetDir() { return m_dir; }

        const char *GetScene() { return m_scene; }

        ErrCodeE SetScene(const char *scene);

        ErrCodeE GetParam(const char *name, Json::Value &json);

        ErrCodeE SetParam(const char *name, Json::Value &json);

        ErrCodeE GetCfg(const char *name, Json::Value &json);

        ErrCodeE GetRange(const char *name, Json::Value &json);

        static bool CheckRangeLegal(const char *name, int param, Json::Value &json);

        static bool CheckItemLegal(const char *name, int param, Json::Value &json);

    private:
        char m_dir[EMX_MAX_PATH_SIZE];
        char m_scene[EMX_MAX_PATH_SIZE];
    };
}

#endif //EMX_MediaServerConfigs_HPP
