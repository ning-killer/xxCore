/**
 * @file ManageCfgFile
 * @author hening
 * @brief 管理1.0框架下Cfg文件
 * @date 2023-5-10
 */

#ifndef EMXFTTOOL_MANAGECFGFILE_HPP
#define EMXFTTOOL_MANAGECFGFILE_HPP

#include "EmxCore.hpp"
#include "FtToolInner.hpp"
#include "AgeingInner.hpp"
#include "EmxBsp.hpp"
#include "SDCardClient.hpp"

namespace Emx {
    class ManageCfgFile {
      public:
        ManageCfgFile(EuvLoop *loop);
        virtual ~ManageCfgFile();

      public:
        void Load();
        void Start();
        FactoryMode GetFactoryMode() {
          return m_factory_mode;
        }

      private:
        void ImportCfgPath();
        void InitFactoryMode();
        bool GetZnDebugConfig(ZnDebugInfo *info);
        bool LoadLogCfg();
        bool LoadIpCfg();
        bool LoadAutoWifiCfg();
        void OnSDCardInfoEvent(int chn, SDCard::Info &info);
        void Guide();

      private:
        CfgFilePath m_cfg_path;
        EuvLoop *m_loop;
        FactoryMode m_factory_mode;
        EuvTimer m_lightTimer;
        Bsp m_bsp;
        bool m_isOnled;
        char m_buffer[256];
        struct SDCardStat {
            bool normal;
            int chn;
            SDCardClientEvent *event;
        } m_sdcard;
        Log::Config m_logConfig;
    };
}

#endif //EMXFTTOOL_MANAGECFGFILE_HPP
