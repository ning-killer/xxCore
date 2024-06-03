//
// Created by xiong on 2022/6/13.
//

#ifndef EMX_SDCardDev_HPP
#define EMX_SDCardDev_HPP

#include "SDCard.hpp"
#include "SDCardResource.hpp"
#include <linux/netlink.h>
#include <mutex>

namespace Emx {

    class SDCardDev : public SDCard {
    public:
        SDCardDev(SDCardResource &res, Json::Value &cfg, int chn);

        virtual ~SDCardDev() {}

        void Create();

        void Destroy();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

    private:
        ErrCodeE Mount() override;

        ErrCodeE UnMount() override;

        ErrCodeE Format() override;

        ErrCodeE GetInfo(Info &info) override;

        ErrCodeE Check() override;

    private:
        void InitStat();

        void Publish();

        void OnMonitor();

        void OnTryingUnmount();

        void OnTryingMount();

        void OnFormat(void *arg);

        void OnFormatDone(ErrCodeE e, void *arg);

        bool IsMounted();

        int GetPartitionNum(void);

        ErrCodeE DoMount();

        ErrCodeE GetSize();

        void ClearSize() {
            m_info.total = 0;
            m_info.used = 0;
            m_info.free = 0;
        }

        void SelectPartMain();

        int Mount(const char* dev_path, const char* mount_point);

        SDCard::FileSystemE GetDevType(const char* mount_point);

    private:
        //arg
        SDCardResource &m_res;

        Info m_info;

        //hot plug monitor
        EuvPoll m_poll;
        int m_sock;
        sockaddr_nl m_sa;
        iovec m_iov;
        msghdr m_msg;

        EuvTimer m_timer;

        EuvWork m_formatWork;
        ErrCodeE m_formatResult;

        struct Part {
            char devPath[Info::MaxDevSize];
            const char *blk;
        } m_part, m_partBak;
        Part *m_partMain;
        char m_devMajor[Info::MaxDevSize];
        bool m_optMask;
        bool m_autoMount;
        bool m_forceMount;
        bool m_forceMerge;
        uint8_t m_tryCnt;
        std::mutex m_locker;
        bool m_isFdiskFormat;
    };

}

#endif //EMX_SDCardDev_HPP
