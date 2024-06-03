/*
* @Author: xiong
* @Date: 2023/4/4
*/

#include "SDCardUpdate.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#include "EmxMedia.hpp"
#include <time.h>

using namespace Emx;

void SDCardUpdate::Create(bool newThread) {
    if (LoadCfg() != ErrCodeE::Success)
        return;
    m_loop.Init("SDCardUpdate", [this]() {
        m_sdcard->Destroy();
        delete m_sdcard;
        m_update->Destroy();
        delete m_update;
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    m_update = new UpdateEvent;
    m_update->Create(&m_loop, m_buffer, (int) sizeof(m_buffer),
                     std::bind(&SDCardUpdate::OnUpdateEvent, this, ph_1));

    //启用线程检测进程启动时刻是否存在rom.bin文件，如果存在则直接升级；线程结束后再启动SD卡监测
    EuvWork *handle = new EuvWork;
    handle->Create(&m_loop, handle,
                   [this](void *arg) {
                       if (CheckRom()) {
                           Update();
                       }else {
                           emxlogi("rom check failed\n");
                       }
                   },
                   [this](ErrCodeE e, void *arg) {
                       EuvWork* handle = (EuvWork*)arg;
                       m_sdcard = new SDCardClientEvent(m_loop, m_buffer, (int) sizeof(m_buffer),
                                                        std::bind(&SDCardUpdate::OnSDCardEvent, this, ph_1, ph_2));
                       m_sdcard->Create();
                       if (handle != nullptr) {
                           handle->Destroy();
                           delete handle;
                           handle = nullptr;
                       }
                   });
    handle->Run();
    m_loop.Start(newThread);
}


void SDCardUpdate::Destroy() {
    m_loop.StopAndDeInit();
}

ErrCodeE SDCardUpdate::LoadCfg() {
    char path[EMX_MAX_PATH_SIZE];
    snprintf(path, sizeof(path), "%s/sdcardUpdate.json", Misc::GetAppConfigsDir());
    Json::Value json;
    if (EasyJson::Load(path, json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path);
        return ErrCodeE::Failure;
    }
    m_rom = json["rom"].asString();
    m_resultDir = json["resultDir"].asString();
    m_cfg = json;
    return ErrCodeE::Success;
}

bool SDCardUpdate::RomExist() {
    if (File::Exist(m_rom.c_str())) {
        emxlogi("found %s\n", m_rom.c_str());
        return true;
    } else {
        return false;
    }
}

bool SDCardUpdate::RomInfoMatch() {
    FILE *fp = fopen(m_rom.c_str(), "rb");
    if (!fp) {
        emxloge("open %s failed\n", m_rom.c_str());
        return false;
    }
    fseek(fp, 32 + 4, SEEK_SET); //  skip size & md5
    int bufferSize = 64 * 1024;
    auto buffer = (char *) malloc(bufferSize);
    if (!buffer) {
        emxloge("malloc failed\n");
        fclose(fp);
        return false;
    }
    int n;
    int off = 0;
    while ((n = (int) fread(buffer + off, 1, bufferSize - off, fp)) > 0) {
        off += n;
    }
    fclose(fp);

    uint32_t teaCipher[32] = {
            0x5479a2eb, 0x9425a0c9, 0x0194cd5d, 0x6ccc85c7, 0x2aa38a72, 0x14186724, 0x29b14b0a, 0x00eafeae,
            0x040623b1, 0xc8fd1f5d, 0x2689e72d, 0x1df579b6, 0x19269746, 0x32b56a74, 0xf2bbc3dc, 0xdfe235ac,
            0x31032615, 0xc8a974d1, 0xb7695193, 0x316b8fe6, 0xe92d208a, 0x832d204a, 0xf42d1b09, 0x849f9eae,
            0xc1df5393, 0xa39054ca, 0x4c500a46, 0x68bcb6e1, 0xe488be5a, 0xff4d2f4e, 0x14bbccf5, 0x65073298};
    uint8_t key[16];
    uint8_t iv[16];
    TEA tea;
    tea.Decrypt(teaCipher, key, iv);
    AES_CBC aes;
    aes.Init(key, iv);
    aes.Decrypt((uint8_t *) buffer, off);

    uint32_t romJsonSize;
    memcpy(&romJsonSize, buffer, 4);
    romJsonSize = ntohl(romJsonSize);
    if (romJsonSize > (64 * 1024 - 4)) {
        emxloge("rom json size %d > 64KB\n", romJsonSize);
        free(buffer);
        return false;
    }

    Json::Value json;
    if (ErrCodeE::Success != EasyJson::Parse(buffer + 4, (int) romJsonSize, json)) {
        emxloge("rom json size %d > 64KB\n", romJsonSize);
        free(buffer);
        return false;
    }
    free(buffer);

    DevInfo devInfo;
    if (devInfo.Load() != ErrCodeE::Success) {
        emxloge("load dev info failed\n");
        return false;
    }
    //判断类型是否与本机相同
    if (strcmp(devInfo.GetInner()->type, json["type"].asCString()) != 0) {
        emxloge("type not match rom[%s] local[%s]\n", devInfo.GetInner()->type, json["type"].asCString());
        return false;
    }
    //判断版本号是否与本机相同
    if (strcmp(devInfo.GetInner()->version, json["version"].asCString()) == 0) {
        emxloge("version not changed [%s]\n", devInfo.GetInner()->version);
        return false;
    }
    emxlogi("rom info matched\n");
    return true;
}

bool SDCardUpdate::RomMd5Match() {
    FILE *fp = fopen(m_rom.c_str(), "rb");
    if (!fp) {
        emxloge("open %s failed\n", m_rom.c_str());
        return false;
    }
    char md5[32];
    fseek(fp, 4, SEEK_CUR); // skip size
    fread(md5, 32, 1, fp); // read md5
    MD5 calcMd5;
    char buffer[4096];
    int n = 0;
    while ((n = (int) fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        calcMd5.update(buffer, n);
    }
    fclose(fp);
    calcMd5.finalize();
    std::string actual = calcMd5.hexdigest();
    if (strncmp(md5, actual.c_str(), sizeof(md5)) == 0) {
        emxlogi("rom md5 matched\n");
        return true;
    } else {
        emxlogi("rom md5 miss match, received[%.*s], actual[%s]\n", sizeof(md5), md5, actual.c_str());
        return false;
    }
}

void SDCardUpdate::Update() {
    FILE *fp = fopen(m_rom.c_str(), "rb");
    if (!fp) {
        emxloge("open %s failed\n", m_rom.c_str());
        return;
    }
    int bufferSize = 64 * 1024;
    auto buffer = (uint8_t *) malloc(bufferSize);
    if (!buffer) {
        emxloge("malloc failed\n");
        fclose(fp);
        return;
    }
    UpdateClient update;
    update.Create(m_session);
    int n;
    while ((n = (int) fread(buffer, 1, bufferSize, fp)) > 0) {
        if (update.Push(buffer, n) != ErrCodeE::Success)
            break;
    }
    fclose(fp);
    free(buffer);
    update.Destroy();
    m_alreadyUpdate = true;
    emxlogi("alreadyUpdate:%d\n", m_alreadyUpdate);
}

void SDCardUpdate::OnSDCardEvent(int chn, SDCard::Info &info) {
    //已经升级过了 就不要再升级了
    if (m_alreadyUpdate) {
        emxlogw("device is already update, ignore\n");
        return;
    }
    //检测到此标志位文件，则升级;避免插卡检测到rom就升级
    if (m_cfg["updateFlag"].isNull()) {
        emxlogw("not cfg updateFlag, ignore\n");
        return;
    }
    if (File::Exist(m_cfg["updateFlag"].asCString())) {
        if (info.stat == SDCard::StatE::Normal && CheckRom()) {
            // MediaClientServer::DestroyMedia();
            //不再升级前杀进程，进程启动可能不同步,导致kill失败设备被重启了
//        Misc::KillBeforeRun("daemon");
//        Misc::KillBeforeRun("EmxMediaServer");
            Update();
        }
    }
}

void SDCardUpdate::WriteResult(const char *result) {
    Sn sn;
    if (sn.Load() != ErrCodeE::Success) {
        emxloge("load sn failed\n");
    }
    time_t rawtime;
    struct tm *timeinfo;
    char name[EMX_MAX_PATH_SIZE] = {};
    char datetime[EMX_MAX_PATH_SIZE] = {};

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(datetime, EMX_MAX_PATH_SIZE, "%Y%m%d%H%M%S", timeinfo);
    snprintf(name, sizeof(name), "%s/SDCardUpdate_%s_%s_%s", m_resultDir.c_str(), sn.Get(), result, datetime);
    FILE *fp = fopen(name, "wb+");
    if (fp) {
        fclose(fp);
        sync();
    }
}

void SDCardUpdate::LedControl(bool updateStatus) {
    m_alreadyUpdate = true;
    //检测到此标志位文件，则升级后设备不重启;未检测到，设备自动重启
    if (m_cfg["updateFlag"].isNull()) {
        emxlogi("not exist updateFlag, reboot\n");
        Reboot::DoReboot(m_rebootDelayS);
        return;
    }
    if (!File::Exist(m_cfg["updateFlag"].asCString())) {
        emxlogi("not exist flag, reboot\n");
        Reboot::DoReboot(m_rebootDelayS);
    }

    auto &cfg = updateStatus ? m_cfg["updateSuccessful"] : m_cfg["updateFail"];
    if (cfg.empty()) {
        emxloge("load cfg failed\n");
        Reboot::DoReboot(m_rebootDelayS);
        return;
    }

    m_idx = 0;
    m_fastInvert = true;
    m_invMs = cfg["invMs"].asInt();
    m_invFastMs = cfg["invFastMs"].asInt();
    for (auto &item:cfg["allChannels"]) {
        auto gpio = new Gpio(item["num"].asInt(),
                             (Gpio::DirE) item["dir"].asInt(),
                             (Gpio::ActiveE) item["active"].asInt(),
                             (Gpio::ValueE) item["default"].asInt());
        m_allChannels.emplace_back(gpio);
    }
    for (auto &item:cfg["channelsCycleIdx"]) {
        m_channelsCycleIdx.emplace_back(item.asInt());
    }

    m_timer.Create(m_loop);
    m_timerFast.Create(m_loop);
    m_timer.Start(0, m_invMs, [this]() {
        m_timerFast.Stop();
        for (auto &gpio: m_allChannels)
            gpio->SetValue(Gpio::ValueE::Disable);

        m_fastInvert = !m_fastInvert;
        if (m_invFastMs > 0) {
            if (m_fastInvert) {
                m_timerFast.Start(0, m_invFastMs, [this]() {
                    for (auto &gpio: m_allChannels)
                        gpio->SetValue(Gpio::ValueE::Disable);
                    if (m_idx < (int) m_channelsCycleIdx.size()) {
                        //控制LED灯态按照1-->0--1-->0状态变化 0->-1->1->-1->2->-1
                        if (m_channelsCycleIdx[m_idx] >= 0)
                            m_allChannels[m_channelsCycleIdx[m_idx]]->SetValue(Gpio::ValueE::Enable);
                        m_idx++;
                        if (m_idx >= (int) m_channelsCycleIdx.size())
                            m_idx = 0;
                    }
                });
            }
        }else {
            if (m_idx < (int) m_channelsCycleIdx.size()) {
                //控制LED灯态按照1-->0--1-->0状态变化 0->-1->1->-1->2->-1
                if (m_channelsCycleIdx[m_idx] >= 0)
                    m_allChannels[m_channelsCycleIdx[m_idx]]->SetValue(Gpio::ValueE::Enable);
                m_idx++;
                if (m_idx >= (int) m_channelsCycleIdx.size())
                    m_idx = 0;
            }
        }
    });
}

void SDCardUpdate::OnUpdateEvent(UpdateEvent::Event &e) {
    if (e.session != m_session)
        return;
    if (e.stat == UpdateEvent::StatE::Done) {
        emxlogd("update done\n");
        WriteResult("success");
        LedControl(true);
    } else if (e.stat == UpdateEvent::StatE::Error) {
        emxlogd("update error\n");
        WriteResult("failed");
        LedControl(false);
    }
}
