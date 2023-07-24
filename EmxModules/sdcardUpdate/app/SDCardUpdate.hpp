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
        SDCardUpdate() : m_sdcard(nullptr) {}

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

    private:
        static const uint32_t m_session = 11534941;
        std::string m_rom;
        std::string m_resultDir;
        EuvLoop m_loop;
        char m_buffer[128];
        SDCardClientEvent *m_sdcard;
        UpdateEvent *m_update;
    };

}

#endif //EMX_SDCardUpdate_HPP
