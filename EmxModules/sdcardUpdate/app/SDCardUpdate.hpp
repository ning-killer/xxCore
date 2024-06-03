/*
* @Author: xiong
* @Date: 2023/4/4
*/

#ifndef EMX_SDCardUpdate_HPP
#define EMX_SDCardUpdate_HPP

#include "SDCardClient.hpp"
#include "UpdateClient.hpp"

namespace Emx {

    class SDCardUpdate {
    public:
        SDCardUpdate() : m_sdcard(nullptr), m_alreadyUpdate(false) {}

        virtual ~SDCardUpdate() {}

        void Create(bool newThread);

        void Destroy();

    private:
        ErrCodeE LoadCfg();

        bool CheckRom() {
            return RomExist() && RomInfoMatch() && RomMd5Match();
        }

    private:
        bool RomExist();

        bool RomInfoMatch();

        bool RomMd5Match();

        void Update();

        void OnSDCardEvent(int chn, SDCard::Info &info);

        void OnUpdateEvent(UpdateEvent::Event &e);

        void WriteResult(const char *result);

        //用于升级完成后，通过LED指示灯展示升级状态
        void LedControl(bool updateStatus);

    private:
        static const uint32_t m_session = 11534941;
        static const uint32_t m_rebootDelayS = 3;
        std::string m_rom;
        std::string m_resultDir;
        EuvLoop m_loop;
        char m_buffer[128];
        SDCardClientEvent *m_sdcard;
        UpdateEvent *m_update;
        EuvTimer m_timer;
        EuvTimer m_timerFast;
        int m_idx;
        int m_invMs;
        int m_invFastMs;
        bool m_fastInvert;
        bool m_alreadyUpdate;
        std::vector<Gpio *> m_allChannels;
        std::vector<int> m_channelsCycleIdx;
        Json::Value m_cfg;
    };

}

#endif //EMX_SDCardUpdate_HPP
