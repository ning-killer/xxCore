/**
 * @file ModulesServer.hpp
 * @author hening
 * @brief 公共服务
 * @date 2023-6-13
 */

#ifndef EmxModules_MoudlesServer_HPP
#define EmxModules_MoudlesServer_HPP

#include "EmxCore.hpp"
#include "NetServerLib.hpp"
// #include "NtpClientLib.hpp"
#include "SDCardServerLib.hpp"
#include "RtspServerLib.hpp"
#include "UpdateServerLib.hpp"
#include "NightVisionServerLib.hpp"
#include "SDCardUpdateLib.hpp"
#include "AgeingInner.hpp"
#include "FtToolInner.hpp"

#define EMX_TOOLS_BUFFER_SIZE 512

namespace Emx {
    class ModulesServer {
      public:
        ModulesServer();
        virtual ~ModulesServer();

      public:
        ErrCodeE Create();
        ErrCodeE Destroy();

      private:
        void CreateSubSystem();
        bool GetZnDebugConfig(ZnDebugInfo *info, const char* path);
        FactoryMode GetFactoryMode();

      private:
        EuvLoop m_loop;
        EuvTimer m_subModules;
        NetServerLib m_netServer;
        // NtpClientLib m_ntpClient;
        SDCardServerLib m_sdcardServer;
        // RtspServerLib m_rtspServer;
        UpdateServerLib m_updateServer;
        NightVisionServerLib m_nightServer;
        SDCardUpdateLib m_sdcardUpdate;
        FactoryMode m_factoryMode;
    };
}

#endif //EmxModules_MoudlesServer_HPP
