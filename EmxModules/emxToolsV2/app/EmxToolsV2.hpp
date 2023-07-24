//
// Created by xiong on 2021/5/20.
//

#ifndef EMX_EmxToolsV1_HPP
#define EMX_EmxToolsV1_HPP

#include "EmxCore.hpp"
#include "Resource.hpp"

namespace Emx {
    class BroadcastService;

    class ConfigService;

    class FileService;

    class EmxToolsV2 {
    public:
        EmxToolsV2();

        virtual ~EmxToolsV2();

        void Create(bool newThread);

        void Destroy();

    private:
        BroadcastService *m_broadcast;
        ConfigService *m_config;
        FileService *m_file;
        Resource m_res;
    };

}

#endif //EMX_EmxToolsV1_HPP
