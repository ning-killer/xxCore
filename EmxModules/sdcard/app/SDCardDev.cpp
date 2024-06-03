//
// Created by xiong on 2022/6/13.
//

#include "SDCardDev.hpp"
#include "SDCardInner.hpp"
#include <sys/mount.h>
#include <sys/vfs.h>

using namespace Emx;

SDCardDev::SDCardDev(SDCardResource &res, Json::Value &cfg, int chn) : SDCard(chn), m_res(res) {
    memset(&m_sa, 0, sizeof(m_sa));
    m_sa.nl_family = AF_NETLINK;
    m_sa.nl_groups = NETLINK_KOBJECT_UEVENT;
    m_sa.nl_pid = 0; //getpid(); both is ok

    memset(&m_iov, 0, sizeof(m_iov));
    m_iov.iov_base = (void *) m_res.buffer;
    m_iov.iov_len = m_res.size;

    memset(&m_msg, 0, sizeof(m_msg));
    m_msg.msg_name = (void *) &m_sa;
    m_msg.msg_namelen = sizeof(m_sa);
    m_msg.msg_iov = &m_iov;
    m_msg.msg_iovlen = 1;

    memset(&m_info, 0, sizeof(m_info));
    m_info.stat = SDCard::StatE::Rejected;
    m_autoMount = false;
    m_optMask = false;

    strncpy(m_info.mountPoint, cfg["mountPoint"].asCString(), sizeof(m_info.mountPoint));
    snprintf(m_part.devPath, sizeof(m_part.devPath), "/dev/%s", cfg["devPart"].asCString());
    m_part.blk = m_part.devPath + strlen("/dev/");
    snprintf(m_partBak.devPath, sizeof(m_partBak.devPath), "/dev/%s", cfg["devPartBak"].asCString());
    m_partBak.blk = m_partBak.devPath + strlen("/dev/");
    snprintf(m_devMajor, sizeof(m_devMajor), "/dev/%s", cfg["devMajor"].asCString());

    m_forceMount = false;
    m_isFdiskFormat = false;
    if (cfg["forceMount"].isBool()) {
        m_forceMount = cfg["forceMount"].asBool();
    }
    m_forceMerge = false;
    if (cfg["forceMerge"].isBool()) {
        m_forceMerge = cfg["forceMerge"].asBool();
    }
    emxlogd("SDCardServer forceMount[%d],forceMerge[%d]\n", m_forceMount,m_forceMerge);
}

void SDCardDev::Create() {
    m_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (m_sock <= 0) {
        emxloge("socket creating failed:%s\n", strerror(errno));
        return;
    } else {
        if (bind(m_sock, (struct sockaddr *) &m_sa, sizeof(m_sa)) == -1) {
            close(m_sock);
            emxloge("bind failed:%s\n", strerror(errno));
            return;
        }
        m_poll.Init(m_res.loop, m_sock);
        m_poll.Start(std::bind(&SDCardDev::OnMonitor, this), nullptr, nullptr);
    }
    m_timer.Create(m_res.loop);
    m_formatWork.Create(&m_res.loop, nullptr, std::bind(&SDCardDev::OnFormat, this, ph_1),
                        std::bind(&SDCardDev::OnFormatDone, this, ph_1, ph_2));
    InitStat();
}

void SDCardDev::InitStat() {
    SelectPartMain();
    //存在sdcard
    if (m_partMain) {
        if (!IsMounted()) {
            //如果当前没有挂载则尝试挂载
            m_info.stat = SDCard::StatE::Inserted;
            m_tryCnt = 3;
            m_timer.Start(0, 1000, std::bind(&SDCardDev::OnTryingMount, this));
        } else {
            //如果当前已经挂载，更新SDCard信息
            m_info.stat = SDCard::StatE::Normal;
            m_info.fileSystem = GetDevType(m_info.mountPoint);
            strncpy(m_info.dev, m_partMain->devPath, sizeof(m_info.dev));
            m_timer.Start(100, 0, [this]() {
                GetSize();
                Publish();
            });
        }
    }
}


void SDCardDev::Destroy() {
    m_timer.Destroy();
    m_formatWork.Destroy();
    m_poll.Stop();
    close(m_sock);
}

ErrCodeE SDCardDev::OnRecvReq(uint32_t module, uint32_t method,
                              const char *data, int32_t size, std::string &out) {
    emxlogt("module[%u],method[%u],data#%s# size=%d\n", module, method, size ? data : " ", size);
    if (m_optMask) {
        emxlogw("resource is busy,method[%u]\n", method);
        return ErrCodeE::ResInUsing;
    }
    switch ((SDCardMethodE) method) {
        case SDCardMethodE::Mount:
            return Mount();
        case SDCardMethodE::UnMount:
            return UnMount();
        case SDCardMethodE::Format:
            return Format();
        case SDCardMethodE::GetInfo: {
            SDCard::Info info = {};
            ErrCodeE e = GetInfo(info);
            out.assign((char *) &info, sizeof(info));
            return e;
        }
        case SDCardMethodE::Check:
            return Check();
        default:
            emxloge("not support module[%u]\n", module);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE SDCardDev::Mount() {
    if (m_info.stat == SDCard::StatE::Normal)
        return ErrCodeE::Success;
    if (m_info.stat == SDCard::StatE::Rejected)
        return ErrCodeE::ResNotExist;
    if (m_info.stat == SDCard::StatE::Formatting)
        return ErrCodeE::ResInUsing;
    if (m_info.stat == SDCard::StatE::Inserted) {
        m_timer.Stop();
        if (DoMount() == ErrCodeE::Success) {
            m_info.stat = SDCard::StatE::Normal;
            GetSize();
            Publish();
        }
    }
    return ErrCodeE::Failure;

}

ErrCodeE SDCardDev::UnMount() {
    if (m_info.stat == SDCard::StatE::Rejected)
        return ErrCodeE::ResNotExist;
    if (m_info.stat == SDCard::StatE::Formatting)
        return ErrCodeE::ResInUsing;
    if (m_info.stat == SDCard::StatE::Normal) {
        m_info.stat = SDCard::StatE::Inserted;
        ClearSize();
        Publish();
        m_autoMount = false;
        m_timer.Start(0, 1000, std::bind(&SDCardDev::OnTryingUnmount, this));
    }
    return ErrCodeE::Success;
}

ErrCodeE SDCardDev::Format() {
    if (m_info.stat == SDCard::StatE::Rejected)
        return ErrCodeE::ResNotExist;
    auto last = m_info.stat;
    m_info.stat = SDCard::StatE::Formatting;
    if (last == SDCard::StatE::Normal) {
        ClearSize();
        Publish();
    }
    m_timer.Start(1000, 1000, std::bind(&SDCardDev::OnTryingUnmount, this));
    return ErrCodeE::Success;
}

ErrCodeE SDCardDev::GetInfo(SDCard::Info &info) {
    GetSize();
    if (m_info.stat == SDCard::StatE::Rejected) {
        ClearSize();
        m_info.fileSystem = FileSystemE::None;
    }
    memcpy(&info, &m_info, sizeof(info));
    return ErrCodeE::Success;
}

ErrCodeE SDCardDev::Check() {
    std::lock_guard<std::mutex> locker(m_locker);
    char path[32] = { 0 };
    snprintf(path, sizeof(path) - 1, "%s/check", m_info.mountPoint);
    FILE *fp = fopen(path, "wb+");
    if (fp == nullptr) {
        emxloge("create sdcard check file failed\n");
        return ErrCodeE::OpenFailed;
    }
    char data[256] = { 0 };
    for (int i = 0; i < (int)sizeof(data); ++i) {
        data[i] = i;
    }
    fwrite(data, 1, sizeof(data), fp);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);

    fp = fopen(path, "rb");
    if (fp == nullptr) {
        emxloge("create sdcard check file failed\n");
        return ErrCodeE::OpenFailed;
    }
    memset(data, 0, sizeof(data));
    fread(data, 1, sizeof(data), fp);
    fclose(fp);
    fp = nullptr;

    for (int i = 0; i < (int)sizeof(data); ++i) {
        if (data[i] != i) {
            emxloge("verify check file failed\n");
            return ErrCodeE::VerifyFailed;;
        }
    }
    remove(path);
    emxlogd("sdcard check successed\n");
    return ErrCodeE::Success;
}

void SDCardDev::OnMonitor() {
    //it will received msg when the fdisk cmd of format was done
    if (m_info.stat == SDCard::StatE::Formatting)
        return;
    int size = recvmsg(m_sock, &m_msg, 0);
    if (size <= 0)
        return;
//    printf("#%s#\n", m_res.buffer);
    if (strncmp(m_res.buffer, "add@", 4) == 0) {
        emxlogd("%s\n", m_res.buffer);
        if (strstr(m_res.buffer, m_partBak.blk) || strstr(m_res.buffer, m_part.blk)) {
            m_info.stat = SDCard::StatE::Inserted;
            m_optMask = true;
            m_autoMount = true;
        }
    } else if (strncmp(m_res.buffer, "remove@", 7) == 0) {
        if (strstr(m_res.buffer, m_partBak.blk) || strstr(m_res.buffer, m_part.blk)) {
            emxlogd("%s\n", m_res.buffer);
            SDCard::StatE last = m_info.stat;
            m_info.stat = SDCard::StatE::Rejected;
            if (last == SDCard::StatE::Normal) {
                ClearSize();
                Publish();
            }
            m_optMask = true;
        }
    }
    if (m_optMask) {
        //delay 3s then exec hot-plug operation
        //try to umount sdcard, what ever the current stat is
        m_timer.Start(3000, 1000, std::bind(&SDCardDev::OnTryingUnmount, this));
    }
}

void SDCardDev::OnTryingUnmount() {
    emxlogd("trying unmount\n");
    umount(m_info.mountPoint);
    if (!IsMounted()) {
        emxlogd("unmount done\n");
        m_timer.Stop();
        SelectPartMain();
        if (m_info.stat == SDCard::StatE::Formatting) {
            m_formatResult = ErrCodeE::Failure;
            m_formatWork.Run();
        } else if (m_autoMount && m_info.stat == SDCard::StatE::Inserted) {
            //if the sdcard is exist, then try to mount it
            m_tryCnt = 3;
            m_timer.Start(0, 1000, std::bind(&SDCardDev::OnTryingMount, this));
        } else {
            m_optMask = false;
        }
    } else {
        if (m_forceMount) {
            // 卸载失败，检测sdcard是否重新插入，并进行挂载
            if (m_autoMount) {
                if (m_info.stat == SDCard::StatE::Inserted) {
                    //if the sdcard is exist, then try to mount it
                    m_timer.Stop();
                    SelectPartMain();
                    m_tryCnt = 3;
                    m_timer.Start(0, 1000, std::bind(&SDCardDev::OnTryingMount, this));
                }
            }
        }
    }
}

void SDCardDev::OnTryingMount() {
    emxlogd("trying mount\n");
    if (DoMount() == ErrCodeE::Success) {
        if (GetSize() == ErrCodeE::Success) {
            m_info.stat = SDCard::StatE::Normal;
            Publish();
            emxlogd("mount done\n");
        }
        m_tryCnt = 0;
    } else {
        m_tryCnt--;
    }
    if (m_tryCnt == 0) {
        m_timer.Stop();
        m_optMask = false;
    }
}

void SDCardDev::OnFormat(void *arg) {
    if (m_partMain) {
        m_isFdiskFormat = false;
        emxlogi("formatting %s ...\n", m_partMain->devPath);
        if ((m_partMain != &m_part) ||
           ( m_forceMerge && (GetPartitionNum() >2)) ) {
            m_formatResult = Cmd::RunCheck("fdisk.sh %s", m_devMajor);
            if (m_formatResult != ErrCodeE::Success)
                return;
            Cmd::RunCheck("mdev -s");
            /*while (cnt--) {
                emxlogd("trying update dev table:%s\n", 10 - cnt,m_part.devPath);
                int fd = open(m_part.devPath, O_RDONLY);
                ioctl(fd, BLKRRPART, nullptr);
                close(fd);
                sleep(1);
                if (File::Exist(m_part.devPath)) {
                    break;
                }
            }*/
            m_partMain = &m_part;
            m_isFdiskFormat = true;
        }
        m_formatResult = Cmd::RunCheck("mkfs.sh %s", m_partMain->devPath);
    } else {
        emxloge("part main == null\n");
    }
}

void SDCardDev::OnFormatDone(ErrCodeE e, void *arg) {
    m_info.stat = SDCard::StatE::Inserted;
    if (m_formatResult == ErrCodeE::Success) {
        emxlogd("format successful;isFdiskFormat[%d]\n", m_isFdiskFormat);
        m_tryCnt = 3;
        if (!m_isFdiskFormat) {
        m_timer.Start(0, 1000, std::bind(&SDCardDev::OnTryingMount, this));
        }
    } else {
        emxloge("format failed\n");
    }
}

bool SDCardDev::IsMounted() {
    FILE *fp = fopen("/proc/mounts", "r");
    if (!fp) {
        emxloge("cannot open /proc/mounts\n");
        return false;
    }
    bool find = false;
    while (fgets(m_res.buffer, m_res.size, fp)) {
        char *p = strstr(m_res.buffer, m_info.mountPoint);
        if (p && (p[strlen(m_info.mountPoint)] == ' ' || p[strlen(m_info.mountPoint)] == '\t')) {
            find = true;
            break;
        }
    }
    fclose(fp);
    return find;
}

ErrCodeE SDCardDev::DoMount() {
    int ret = -1;
    if (m_partMain) {
        ret = Mount(m_partMain->devPath, m_info.mountPoint);
        if (ret)
            emxloge("mount %s ret = %d,error=%d:%s\n", m_partMain->devPath, ret, errno, strerror(errno));
        else
            strncpy(m_info.dev, m_partMain->devPath, sizeof(m_info.dev));
    }
    if (ret != 0) {
        memset(m_info.dev, 0, sizeof(m_info.dev));
        return ErrCodeE::Failure;
    }
    m_info.fileSystem = GetDevType(m_info.mountPoint);
    return ErrCodeE::Success;
}

int SDCardDev::Mount(const char* dev_path, const char* mount_point) {
    int ret = -1;
    do {
        if (dev_path == nullptr || mount_point == nullptr) {
            break;
        }

        char mount_cmd[256] = { 0 };
        snprintf(mount_cmd, sizeof(mount_cmd) - 1
                , "mount %s %s -o errors=continue && mount -o remount,rw %s"
                , dev_path, mount_point, dev_path);
        if (Cmd::RunCheck(mount_cmd) != ErrCodeE::Success) {
            break;
        }

        char check_cmd[64] = { 0 };
        snprintf(check_cmd, sizeof(check_cmd) - 1
                , "df | grep %s", mount_point);
        ret = (int)Cmd::RunCheck(check_cmd);
    } while(false);
    emxlogd("sdcard mount %s\n", ret == 0 ? "success" : "failed");
    return ret;
}

SDCard::FileSystemE SDCardDev::GetDevType(const char* mount_point) {
    FileSystemE type = SDCard::FileSystemE::None;
    if (mount_point == nullptr) {
        return type;
    }
    char temp[1024] = { 0 };
    std::string str = "";
    snprintf(temp, sizeof(temp) - 1, "mount | grep %s | grep -v grep", mount_point);
    FILE *pp = popen(temp, "r");
    if (pp) {
        memset(temp, 0, sizeof(temp));
        while (fgets(temp, sizeof(temp), pp)) {
            if (strstr(temp, mount_point)) {
                if (strstr(temp, "exfat")) {
                    type = SDCard::FileSystemE::Fat64;
                } else if (strstr(temp, "vfat")) {
                    type = SDCard::FileSystemE::Fat32;
                } else if (strstr(temp, "ntfs")) {
                    type = SDCard::FileSystemE::Ntfs;
                } else {
                    type = SDCard::FileSystemE::None;
                }
                break;
            }
        }
        pclose(pp);
    }
    return type;
}


//this function will block for about xx ms,test on hi3518ev300
ErrCodeE SDCardDev::GetSize() {
    TimeRec cost;
    struct statfs diskInfo = {};
    m_info.total = 0;
    m_info.used = 0;
    m_info.free = 0;
    if (statfs(m_info.mountPoint, &diskInfo)) {
        emxloge("cannot get sdcard size %d:%s\n", errno, strerror(errno));
        return ErrCodeE::Failure;
    }
    auto f_bsize = (unsigned long long) diskInfo.f_bsize;
    auto f_blocks = (unsigned long long) diskInfo.f_blocks;
    auto f_bavail = (unsigned long long) diskInfo.f_bavail;
    auto total = f_bsize * f_blocks;
    auto free = f_bsize * f_bavail;
    if (free > total) {
        /*当tf卡free为0时，软件继续写入数据会有几率造成free为一个负数，unsigned 会变会最大值*/
        free = 0;
    }
    total = total >> 20;
    free = free >> 20;
    m_info.total = (unsigned int) total;
    m_info.free = (unsigned int) free;
    m_info.used = (unsigned int) total - (unsigned int) free;
    auto inv = cost.GetInv();
    if (inv > 100)
        emxlogi("total=%u,free=%u,used=%u,cost=%ums\n", m_info.total, m_info.free, m_info.used, inv);
    return ErrCodeE::Success;
}

void SDCardDev::Publish() {
    char buffer[sizeof(m_chn) + sizeof(m_info)];
    memcpy(buffer, &m_chn, sizeof(m_chn));
    memcpy(buffer + sizeof(m_chn), &m_info, sizeof(m_info));
    m_res.pub.Publish(SDCARD_TOPIC_ID, buffer, sizeof(m_chn) + sizeof(m_info));
}

void SDCardDev::SelectPartMain() {
    if (File::Exist(m_part.devPath))
        m_partMain = &m_part;
    else if (File::Exist(m_partBak.devPath))
        m_partMain = &m_partBak;
    else
        m_partMain = nullptr;
}

int SDCardDev::GetPartitionNum(void) {
    FILE *file = fopen("/proc/partitions", "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open /proc/partitions\n");
        return 0;
    }

    char line[64];
    int sdcardPartitions = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "mmcblk0") != NULL) {
            char dev[64];
            int major = 0, minor = 0, blocks = 0;
            sscanf(line, "%d %d %d %s", &major, &minor, &blocks, dev);
            if (strstr(dev, "mmcblk0") != NULL) {
                emxlogi("Found partition for %s, major: %d, minor: %d, blocks: %d\n", dev, major, minor, blocks);
                sdcardPartitions++;
            }
        }
    }

    fclose(file);
    emxlogi("Total partitions for the SD card: %d\n", sdcardPartitions);
    return sdcardPartitions;
}
