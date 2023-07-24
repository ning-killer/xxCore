/*
 * @Author: xiong
 * @Date: 2022/8/26
*/

#include "UpdateServer.hpp"
#include "UpdateInner.hpp"
#include "FlashFactory.hpp"
#include "DevInfo.hpp"
#include "BootEnv.hpp"
#include <fstream>
#include <iostream>

using namespace Emx;

UpdateServer::UpdateServer(): 
    m_updateWay(Burn_After_Downloading)
    , m_deBlockSize(EMX_UPDATE_DEFAULT_BLOCK_SIZE)
    , m_burnBlockSize(EMX_UPDATE_DEFAULT_BLOCK_SIZE) {
}

void UpdateServer::Create(bool newThread) {
    if (ParseConfig() != ErrCodeE::Success)
        return;

    if (GetBootEnvInfo() != ErrCodeE::Success)
        return;

    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock <= 0) {
        emxloge("failed:%d:%s\n", errno, strerror(errno));
        return;
    }
    sockaddr_un un = {};
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s", EMX_DOMAIN_UPDATE_SERVER);
    unlink(un.sun_path);
    if (bind(sock, (sockaddr *) &un, sizeof(un)) != 0) {
        emxloge("failed:%d:%s\n", errno, strerror(errno));
        close(sock);
        return;
    }
    m_loop.Init("UpdateServer", [this]() {
        ResetContext();
        m_udp.Destroy();
        m_timer.Destroy();
        delete m_flash;
    },  newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    m_udp.Create(m_loop);
    m_udp.OpenExistSock(sock);
    m_udp.StartRecv((char *) m_buffer, sizeof(m_buffer),
                    std::bind(&UpdateServer::OnRecv, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3, std::placeholders::_4));
    m_timer.Create(m_loop);
    memset(&m_event, 0, sizeof(m_event));
    ResetContext();
    emxlogd("UpdateServer newThread(%d)\n",newThread);
    m_loop.Start(newThread);
}

void UpdateServer::Destroy() {
    m_loop.StopAndDeInit();
}

static unsigned long long ParseSize(const char *ptr, char **retptr) {
    char *endPtr;    /* local pointer to end of parsed string */
    unsigned long long ret = strtoull(ptr, &endPtr, 0);
    switch (*endPtr) {
        case 'E':
        case 'e':
            ret <<= 10;
        case 'P':
        case 'p':
            ret <<= 10;
        case 'T':
        case 't':
            ret <<= 10;
        case 'G':
        case 'g':
            ret <<= 10;
        case 'M':
        case 'm':
            ret <<= 10;
        case 'K':
        case 'k':
            ret <<= 10;
            endPtr++;
        default:
            break;
    }
    if (retptr)
        *retptr = endPtr;
    return ret;
}

ErrCodeE UpdateServer::GetBootEnvInfo() {
    std::string partitionString;
    if (BootEnv::GetEnv("partition", partitionString) != ErrCodeE::Success) {
        emxlogc("cannot found partition in boot env\n");
        return ErrCodeE::ResNotExist;
    }
    //    "partition=boot:384K,env:128K,kernel:2432K,rootfs:1M,media:5824K,app.A:3M,app.B:3M,data:448K"
//    char *pSave;
//    char *pEach;
//    char *pPartition = (char *) partition.data();
//    while ((pEach = strtok_r(pPartition, "\n", &pSave))) {
//        pPartition = nullptr;
//        Partition part = {};
//        char *pName;
//        char *pSize;
//        //name
//        pName = strtok_r(pEach, ":", &pSize);
//        strncpy(part.name, pName, sizeof(part.name));
//        part.size = (int) ParseSize(pSize, &pSize);
//    }
    Json::Value partitionJson;
    if (EasyJson::Parse(partitionString.data(), partitionJson) != ErrCodeE::Success) {
        emxlogc("parse partition in boot env failed#%s#\n", partitionString.c_str());
        return ErrCodeE::ResNotExist;
    }

    for (auto &part : partitionJson["partitionTable"]) {
        Partition p = {};
        strncpy(p.name, part["name"].asCString(), sizeof(p.name));
        auto partSizeString = part["size"].asString();
        char *pSize = (char *) partSizeString.c_str();
        if (strstr(pSize, "-"))
            continue;
        p.size = (int) ParseSize(pSize, &pSize);
        m_partition.push_back(p);
        emxlogd("part[%s] size[%d]\n", p.name, p.size);
    }

    if (m_mode == ModeE::Spare) {
        std::string spare;
        if (BootEnv::GetEnv("spare", spare) != ErrCodeE::Success) {
            emxlogc("cannot found spare in boot env\n");
            return ErrCodeE::ResNotExist;
        }
        m_systemSpare = spare == "0" ? SystemSpareE::Normal : SystemSpareE::Spare;
        emxlogi("current spare select %s\n", m_systemSpare == SystemSpareE::Normal ? "Normal" : "Spare");
    } else {
        std::string systemSelect;
        if (BootEnv::GetEnv("systemSelect", systemSelect) != ErrCodeE::Success) {
            emxlogc("cannot found systemSelect in boot env\n");
            return ErrCodeE::ResNotExist;
        }
        m_systemSelect = systemSelect == "A" ? SystemSelectE::A : SystemSelectE::B;
        emxlogi("current system select %s\n", systemSelect.c_str());
    }

    return ErrCodeE::Success;
}

ErrCodeE UpdateServer::ParseConfig() {
    Json::Value json;
    auto path = std::string(Misc::GetAppConfigsDir());
    path.append("/update.json");
    if (!File::Exist(path.c_str())) {
        printf("%s not exist\n", path.c_str());
        return ErrCodeE::Failure;
    }
    if (EasyJson::Load(path.c_str(), json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path.c_str());
        return ErrCodeE::Failure;
    }
    m_flash = FlashFactory::Make(json["flashType"].asCString());
    if (!m_flash) {
        emxloge("create flash failed\n");
        return ErrCodeE::Failure;
    }

    if (json["mode"].asString() == "doubleSystem") {
        m_mode = ModeE::DoubleSystem;
    } else {
        m_mode = ModeE::Spare;
    }
    emxlogi("UpdateServer Running On %s Mode\n", m_mode == ModeE::Spare ? "Spare" : "DoubleSystem");

    m_dev = json["dev"].asString();
    m_firstPart = json["firstPart"].asInt();
    m_rebootAfterComplete = false;
    if (json.isMember("rebootAfterComplete")) {
        m_rebootAfterComplete = json["rebootAfterComplete"].asBool();
        m_rebootDelayS = json["rebootDelayS"].asInt();
    }

    if (json["updateWay"].isInt()) {
        m_updateWay = (UpdateWayE)json["updateWay"].asInt();
    }

    if (json["BlockSize"].isInt()) {
        m_burnBlockSize = json["BlockSize"].asInt();
        m_deBlockSize = m_burnBlockSize;
    }
    emxlogd("update way: %d; block size: %d\n", m_updateWay, m_burnBlockSize);
    return ErrCodeE::Success;
}

void UpdateServer::OnRecv(ErrCodeE e, const char *data, int32_t size, const struct sockaddr *addr) {
    if (e != ErrCodeE::Success || data == nullptr || size < (int32_t) sizeof(UpdatePackHeader)) {
        emxloge("error e = %d,addr = %p, data = %p, size = %d\n", e, addr, data, size);
        SendAck(addr, ErrCodeE::ParseFailed);
        return;
    }
    UpdatePackHeader header = {};
    memcpy(&header, data, sizeof(header));
    if (header.sync != EMX_UPDATE_PACK_SYNC) {
        emxloge("header sync %#x failed\n", header.sync);
        SendAck(addr, ErrCodeE::ParseFailed);
        return;
    }
    data += sizeof(header);
    size -= sizeof(header);

    if (!m_updating) {
        //idle stat
        //the first data seq should be 0 when new session connected
        if (header.seq != 0) {
            SendAck(addr, ErrCodeE::Failure);
            return;
        }
        m_updating = true;
        m_session = header.session;
        emxlogi("accept session[%d], downloading...\n", header.session);
        m_waitSeq = 0;
        if (m_updateWay == Burn_While_Downloading) {
            InitDecrypt();
        }
    }

    //updating stat
    //if pack from others ,drop it
    if (header.session != m_session) {
        emxloge("is updating by session[%d], drop session[%d]\n", m_session, header.session);
        SendAck(addr, ErrCodeE::ResInUsing);
        return;
    }

    if (header.seq != m_waitSeq) {
        emxloge("session[%d] missing seq, expected[%u] actual[%u]\n", m_session, m_waitSeq, header.seq);
        SendAck(addr, ErrCodeE::Failure);
        //只有当发布过开始后才会发布错误，否则默默退出
        //只有接收完头信息才算是正式开始
        if (m_event.stat == UpdateEvent::StatE::Start) {
            m_event.stat = UpdateEvent::StatE::Error;
            PublishEvent();
        }
        ResetContext();
        return;
    }
    m_waitSeq++;

    //接收到session的数据，刷新超时定时器
    UpdateTimer();

    if (m_updateWay == Burn_After_Downloading) {
        do {
            ErrCodeE ret = Download(data, size, addr);
            //接收成功
            if (ret == ErrCodeE::Success)
                return;
            //接受失败
            if (ret != ErrCodeE::ReachTheEof)
                break;
            //接收到完整的固件包,停止超时定时器
            m_timer.Stop();
            //开始校验
            if (Verify() != ErrCodeE::Success)
                break;
            //开始解密
            Decrypt();
            //准备烧录
            if (PrepareBurn(m_data) != ErrCodeE::Success)
                break;
            //烧录
            if (Burn() == ErrCodeE::Success) {
                if (m_mode == ModeE::DoubleSystem)
                    BootEnv::SetEnv("systemSelect", m_systemSelect == SystemSelectE::A ? "B" : "A");
                else
                    BootEnv::SetEnv("spare", "0");
                emxlogi("update successful\n");
            }
            break;
        } while (false);
        ResetContext();
    } else if (m_updateWay == Burn_While_Downloading) {
        StreamingUpdate(data, size, addr);
    } else if (m_updateWay == Burn_After_DownloadtoFile) {
        emxloge("not support this update way: Burn_After_DownloadtoFile!\n");
    } else {
        emxloge("no this update way: %d!\n", (int)m_updateWay);
    }
}

ErrCodeE UpdateServer::StreamingUpdate(const char *data, int32_t size, const struct sockaddr *addr) {
    ErrCodeE ret = ErrCodeE::Failure;
    do {
        // 下载
        ret = StreamingDownload(data, size, addr);
        if (ret == ErrCodeE::Success) {
            return ret;
        } else if (ret == ErrCodeE::Failure) {
            emxloge("download failed\n");
            break;
        }

        // 解密
        ret = HandleDecrypt(data, size, addr);
        if (ret == ErrCodeE::Success) {
            return ret;
        } else if (ret == ErrCodeE::Failure) {
            emxloge("decrypt failed\n");
            break;
        }

        // 打包可烧录数据块
        ret = PackBlock(data, size, addr);
        if (ret == ErrCodeE::Success) {
            return ret;
        } else if (ret == ErrCodeE::Failure) {
            emxloge("pack block failed\n");
            break;
        }
        m_timer.Stop();

        // 烧录
        do {
            ret = StreamingBurn();
            if (ret == ErrCodeE::Failure) {
                break;
            }
            if (!m_cipherBlock.isReachEnd) {
                // 未到尾部数据只处理一次烧录
                break;
            }
            // 处理尾部数据烧录
            emxlogd("reach end, current cipher[%d], current burn[%d], burn total[%d]\n"
                    , m_cipherBlock.data.size(), m_event.burn.current, m_event.burn.total);
            if (m_event.burn.current == m_event.burn.total) {
                break;
            }
        } while(true);

        if (ret == ErrCodeE::Success) {
            return ret;
        } else if (ret == ErrCodeE::Failure) {
            emxloge("burn failed\n");
            break;
        }

        if (m_mode == ModeE::DoubleSystem) {
            BootEnv::SetEnv("systemSelect", m_systemSelect == SystemSelectE::A ? "B" : "A");
        } else {
            BootEnv::SetEnv("spare", "0");
        }
        emxlogd("update successful\n");
        break;
    } while (false);
    ResetContext();
    return ret;
}

ErrCodeE UpdateServer::Download(const char *data, int32_t size, const struct sockaddr *addr) {
    if (m_waitHeader) {
        //如果文件头没有接收完整
        m_header.append(data, size);
        int waitSize = 32 + 4;//md5 + rom size
        if ((int) m_header.size() < waitSize)
            return ErrCodeE::Success;
        m_waitHeader = false;
        //剩余的数据交给m_data
        data = m_header.data() + waitSize;
        size = (int) m_header.size() - waitSize;
        memcpy(&m_fileHeader.size, m_header.data(), 4);
        m_fileHeader.size = ntohl(m_fileHeader.size);
        memcpy(&m_fileHeader.md5, m_header.data() + 4, 32);

        memset(&m_event, 0, sizeof(m_event));
        m_event.stat = UpdateEvent::StatE::Start;
        m_event.session = m_session;
        m_event.download.total = (int) m_fileHeader.size;
        //发布升级开始的消息，其他模块收到此消息后需要考虑是否释放内存空间
        PublishEvent();
        m_bsp.updateIndicator->Start();
        m_publishSize = m_event.download.total / 50;
    }

    //存放数据
    try {
        m_data.append(data, size);
        m_event.download.current += size;
    } catch (std::exception &exception) {
        emxloge("malloc failed %s\n", exception.what());
        SendAck(addr, ErrCodeE::MallocFailed);
        m_event.stat = UpdateEvent::StatE::Error;
        PublishEvent();
        return ErrCodeE::MallocFailed;
    }

    if (m_data.size() < m_fileHeader.size) {
        //还未接收完整
        SendAck(addr, ErrCodeE::Success);
        if (m_event.download.current >= m_publishSize) {
            PublishEvent();
            m_publishSize += m_event.download.total / 50;
        }
        return ErrCodeE::Success;
    }
    if (m_data.size() > m_fileHeader.size) {
        //接收数据大小超过预期
        emxloge("rom file size now received[%d] larger than expected[%d]\n", m_data.size(), m_fileHeader.size);
        SendAck(addr, ErrCodeE::Failure);
        m_event.stat = UpdateEvent::StatE::Error;
        PublishEvent();
        return ErrCodeE::Failure;
    }
    SendAck(addr, ErrCodeE::ReachTheEof);
    PublishEvent();
    //数据接收完毕开始升级
    return ErrCodeE::ReachTheEof;
}

ErrCodeE UpdateServer::StreamingDownload(const char *data, int32_t size, const struct sockaddr *addr) {
    if (m_waitHeader) {
        //如果文件头没有接收完整
        m_header.append(data, size);
        int waitSize = 32 + 4;//md5 + rom size
        if ((int) m_header.size() < waitSize)
            return ErrCodeE::Success;
        m_waitHeader = false;
        //剩余的数据交给m_data
        data = m_header.data() + waitSize;
        size = (int) m_header.size() - waitSize;
        memcpy(&m_fileHeader.size, m_header.data(), 4);
        m_fileHeader.size = ntohl(m_fileHeader.size);
        memcpy(&m_fileHeader.md5, m_header.data() + 4, 32);

        memset(&m_event, 0, sizeof(m_event));
        m_event.stat = UpdateEvent::StatE::Start;
        m_event.session = m_session;
        m_event.download.total = (int) m_fileHeader.size;
        //发布升级开始的消息，其他模块收到此消息后需要考虑是否释放内存空间
        PublishEvent();
        m_bsp.updateIndicator->Start();
        m_publishSize = m_event.download.total / 50;
    }

    //存放数据
    try {
        m_data.append(data, size);
        emxlogd("total: %d; download current: %d\n", m_fileHeader.size, m_event.download.current);
        m_event.download.current += size;
    } catch (std::exception &exception) {
        emxloge("malloc failed %s\n", exception.what());
        SendAck(addr, ErrCodeE::MallocFailed);
        m_event.stat = UpdateEvent::StatE::Error;
        PublishEvent();
        return ErrCodeE::MallocFailed;
    }

    if (m_event.download.current < (int)m_fileHeader.size) {
        //还未接收完整
        SendAck(addr, ErrCodeE::Success);
        if (m_event.download.current >= m_publishSize) {
            PublishEvent();
            m_publishSize += m_event.download.total / 50;
        }
        m_cipherBlock.isReachEnd = false;
        return ErrCodeE::ReachTheEof;
    }
    if (m_event.download.current > (int)m_fileHeader.size) {
        //接收数据大小超过预期
        emxloge("rom file size now received[%d] larger than expected[%d]\n", m_event.download.current, m_fileHeader.size);
        SendAck(addr, ErrCodeE::Failure);
        m_event.stat = UpdateEvent::StatE::Error;
        PublishEvent();
        return ErrCodeE::Failure;
    }
    // 尾部接收
    m_cipherBlock.isReachEnd = true;
    SendAck(addr, ErrCodeE::ReachTheEof);
    PublishEvent();
    return ErrCodeE::ReachTheEof;
}

ErrCodeE UpdateServer::HandleDecrypt(const char *data, int32_t size, const struct sockaddr *addr) {
    int deSize = m_deBlockSize;
    if ((int)m_data.size() < deSize && !m_cipherBlock.isReachEnd) {
        return ErrCodeE::Success;
    }
    if (m_cipherBlock.isReachEnd) {
        deSize = m_data.size();
    }
    m_cipherBlock.data.append(std::string(m_data.data(), deSize));
    int off = m_cipherBlock.data.size() - deSize;
    Decrypt((char*)m_cipherBlock.data.data() + off, deSize, m_cipherBlock.isReachEnd);
    OffsetStr(m_data, deSize, m_data.size() - deSize);
    return ErrCodeE::ReachTheEof;
}

ErrCodeE UpdateServer::PackBlock(const char *data, int32_t size, const struct sockaddr *addr) {
    int waitSize = 0;
    // romjson接收处理
    if (!m_romJson.isRecved) {
        waitSize = 128 * 1024;
        if ((int)m_cipherBlock.data.size() < waitSize) {
            return ErrCodeE::Success;
        }
        m_romJson.isRecved = true;
        if (PrepareBurn(m_cipherBlock.data) == ErrCodeE::Failure) {
            return ErrCodeE::Failure;
        }
        // 判断是否接收完毕script脚本是否接收完毕
        if (m_cipherBlock.data.size() < m_romJson.size) {
            m_romJson.isRecvedScript = false;
            return ErrCodeE::Success;
        }
        m_romJson.isRecvedScript = true;
        // 移除解密块数据中的romjson+script脚本数据
        OffsetStr(m_cipherBlock.data, m_romJson.size, m_cipherBlock.data.size() - m_romJson.size);
        return ErrCodeE::Success;
    }

    // script脚本接收处理
    if (!m_romJson.isRecvedScript) {
        waitSize = m_romJson.size;
        if ((int)m_cipherBlock.data.size() < waitSize) {
            return ErrCodeE::Success;
        }
        m_romJson.isRecvedScript = true;
        // 重置初次解析数据
        m_romInfo.img.clear();
        m_romInfo.preScriptSize = 0;
        m_romInfo.postScriptSize = 0;
        if (PrepareBurn(m_cipherBlock.data) == ErrCodeE::Failure) {
            return ErrCodeE::Failure;
        }
        // 移除解密块数据中的romjson+script脚本数据
        OffsetStr(m_cipherBlock.data, m_romJson.size, m_cipherBlock.data.size() - m_romJson.size);
        return ErrCodeE::Success;
    }

    waitSize = m_burnBlockSize;
    if ((int)m_cipherBlock.data.size() < waitSize) {
        return ErrCodeE::Success;
    }
    return ErrCodeE::ReachTheEof;
}

ErrCodeE UpdateServer::Verify() {
    emxlogi("rom verifying...\n");
    MD5 md5;
    md5.update(m_data.data(), m_data.size());
    md5.finalize();
    auto total_md5 = md5.hexdigest();
    if (total_md5 != m_fileHeader.md5) {
        emxloge("rom md5 not match,in header[%s] actual[%s]\n", m_fileHeader.md5, total_md5.c_str());
        m_event.stat = UpdateEvent::StatE::Error;
        PublishEvent();
        return ErrCodeE::VerifyFailed;
    }
    return ErrCodeE::Success;
}

void UpdateServer::Decrypt() {
    emxlogi("rom decrypting...\n");
    uint32_t teaCipher[32] = {
            0x5479a2eb, 0x9425a0c9, 0x0194cd5d, 0x6ccc85c7, 0x2aa38a72, 0x14186724, 0x29b14b0a, 0x00eafeae,
            0x040623b1, 0xc8fd1f5d, 0x2689e72d, 0x1df579b6, 0x19269746, 0x32b56a74, 0xf2bbc3dc, 0xdfe235ac,
            0x31032615, 0xc8a974d1, 0xb7695193, 0x316b8fe6, 0xe92d208a, 0x832d204a, 0xf42d1b09, 0x849f9eae,
            0xc1df5393, 0xa39054ca, 0x4c500a46, 0x68bcb6e1, 0xe488be5a, 0xff4d2f4e, 0x14bbccf5, 0x65073298};
    TEA tea;
    uint8_t key[16];
    uint8_t iv[16];
    tea.Decrypt(teaCipher, key, iv);
    AES_CBC aes;
    aes.Init(key, iv);
    aes.Decrypt((uint8_t *) m_data.data(), m_data.size());
    Pkcs7Padding::UnPad(m_data);
}

void UpdateServer::InitDecrypt() {
    uint32_t teaCipher[32] = {
            0x5479a2eb, 0x9425a0c9, 0x0194cd5d, 0x6ccc85c7, 0x2aa38a72, 0x14186724, 0x29b14b0a, 0x00eafeae,
            0x040623b1, 0xc8fd1f5d, 0x2689e72d, 0x1df579b6, 0x19269746, 0x32b56a74, 0xf2bbc3dc, 0xdfe235ac,
            0x31032615, 0xc8a974d1, 0xb7695193, 0x316b8fe6, 0xe92d208a, 0x832d204a, 0xf42d1b09, 0x849f9eae,
            0xc1df5393, 0xa39054ca, 0x4c500a46, 0x68bcb6e1, 0xe488be5a, 0xff4d2f4e, 0x14bbccf5, 0x65073298};
    TEA tea;
    uint8_t key[16];
    uint8_t iv[16];
    tea.Decrypt(teaCipher, key, iv);
    m_cipherBlock.aes.Init(key, iv);
}

void UpdateServer::Decrypt(char* data, int size, bool isNeedUnPad) {
    emxlogi("rom decrypting, %s.\n", isNeedUnPad ? "need unpad" : "no need unpad");
    m_cipherBlock.aes.Decrypt((uint8_t *) data, size);
    if (isNeedUnPad) {
        std::string tmpStr(data, size);
        Pkcs7Padding::UnPad(tmpStr);
        memcpy(data, tmpStr.data(), size);
    }
}

void UpdateServer::OffsetStr(std::string &str, int offset, int size) {
    memcpy((void *)str.data(), str.data() + offset, size);
    str.resize(size);
    str.shrink_to_fit();
}

ErrCodeE UpdateServer::PrepareBurn(const std::string &str) {
    emxlogi("rom pre burning...\n");
    DevInfo devInfo;
    Json::Value romJson;
    int offset = 0;
    uint32_t romJsonSize;
    memcpy(&romJsonSize, str.data(), 4);
    romJsonSize = ntohl(romJsonSize);
    offset += 4;
    if (romJsonSize > (64 * 1024)) {
        emxloge("rom json size %d > 64KB\n", romJsonSize);
        goto ERROR;
    }

    //解析Json
    if (EasyJson::Parse(str.data() + offset, (int) romJsonSize, romJson) != ErrCodeE::Success) {
        emxloge("parse rom json failed\n");
        goto ERROR;
    }
    offset += (int) romJsonSize;

    if (devInfo.Load() != ErrCodeE::Success) {
        emxloge("load dev info failed\n");
        goto ERROR;
    }
    //判断类型是否与本机相同
    if (strcmp(devInfo.GetInner()->type, romJson["type"].asCString()) != 0) {
        emxloge("type not match rom[%s] local[%s]\n", devInfo.GetInner()->type, romJson["type"].asCString());
        goto ERROR;
    }
    //判断版本号是否与本机相同
    if (strcmp(devInfo.GetInner()->version, romJson["version"].asCString()) == 0) {
        emxloge("version not changed [%s]\n", devInfo.GetInner()->version);
        goto ERROR;
    }

    if (romJson.isMember("script")) {
        if (romJson["script"].isMember("preSize")) {
            int size = romJson["script"]["preSize"].asInt();
            if (WriteScript(EMX_UPDATE_PRE_SCRIPT, str.data() + offset, size) != ErrCodeE::Success) {
                goto ERROR;
            }
            offset += size;
            m_romInfo.preScriptSize = size;
        }
        if (romJson["script"].isMember("postSize")) {
            int size = romJson["script"]["postSize"].asInt();
            if (WriteScript(EMX_UPDATE_POST_SCRIPT, str.data() + offset, size) != ErrCodeE::Success) {
                goto ERROR;
            }
            offset += size;
            m_romInfo.postScriptSize = size;
        }
    }

    m_romJson.size = offset;

    if (romJson.isMember("img")) {
        for (auto &part : romJson["img"]) {
            RomInfo::Img img = {};
            img.isOpen = false;
            img.imgOff = 0;
            img.partOff = 0;
            img.name = part["name"].asString();
            img.data = (uint8_t *) str.data() + offset;
            img.size = part["size"].asInt();
            m_event.burn.total += img.size;

            //通过当前img名称寻找目标烧写分区
            //假设所有img都拥有双分区,如果没有双分区也不影响寻找
            char findPartName[16] = {};//"media"
            char findPartName2[16] = {};//"media.B" or "media.A"
            strncpy(findPartName, img.name.c_str(), sizeof(findPartName));
            snprintf(findPartName2, sizeof(findPartName2), "%s.%s",
                     findPartName,
                     m_systemSelect == SystemSelectE::A ? "B" : "A");
            emxlogd("try to find [%s] or [%s] in partition table\n", findPartName, findPartName2);
            for (int i = 0; i < (int) m_partition.size(); i++) {
                if (strcmp(m_partition[i].name, findPartName) == 0 ||
                    strcmp(m_partition[i].name, findPartName2) == 0) {
                    // 通过配置文件提供的分区的前缀和第一个分区号构造设备路径
                    // 例如nand和nor的分区号是从0开始 /dev/mtd0
                    // emmc的分区号是从1开始 /dev/mmcblk0p1'
                    char dev[64] = {};
                    snprintf(dev, sizeof(dev), "%s%d", m_dev.c_str(), m_firstPart + i);
                    img.dev = dev;
                    img.partSize = m_partition[i].size;
                    break;
                }
            }

            if (img.dev.empty()) {
                emxloge("cannot found img name %s or %s in partition table\n", findPartName, findPartName2);
                goto ERROR;
            }

            if (img.size > img.partSize) {
                emxloge("img[%s] size[%s] > partition[%s] size[%]\n",
                        img.name.c_str(), img.size, img.dev.c_str(), img.partSize);
                goto ERROR;
            }
            offset += img.size;
            m_romInfo.img.push_back(img);
        }
    }
    return ErrCodeE::Success;
    ERROR:
    m_event.stat = UpdateEvent::StatE::Error;
    PublishEvent();
    return ErrCodeE::Failure;
}

ErrCodeE UpdateServer::Burn() {
    emxlogi("rom burning...\n");
    m_publishSize = m_event.burn.total / 50;
    if (m_romInfo.preScriptSize) {
        emxlogi("execute %s\n", EMX_UPDATE_PRE_SCRIPT);
        Cmd::System(EMX_UPDATE_PRE_SCRIPT);
    }

    for (auto &img : m_romInfo.img) {
        emxlogi("burning img [%s] to [%s],imgsize [%d], partsize [%d]\n",
                img.name.c_str(), img.dev.c_str(), img.size, img.partSize);
        //如果是spare升级方式，spare img应处于img列表的第一个位置
        if (m_mode == ModeE::Spare && m_systemSpare == SystemSpareE::Spare && img.name == "spare") {
            //如果当前处于spare升级方式，并且运行与spare分区，则不升级spare分区，防止变砖
            m_event.burn.current += img.size;
            emxlogi("skip spare img\n");
            continue;
        }
        // step3.burning
        if (!img.isOpen) {
            if (m_flash->Open(img.dev.c_str()) != ErrCodeE::Success) {
                goto ERROR;
            }
            img.isOpen = true;
        }
        mtd_info_t mtd_info;
        if (m_flash->GetInfo(mtd_info) != ErrCodeE::Success) {
            goto ERROR;
        }
        emxlogd("mtd erasesize: %d; writesize: %d\n", mtd_info.erasesize, mtd_info.writesize);
        emxlogi("[%s] erasing writing and verifying... \n", img.name.c_str());
        int imgSize = img.size;
        int imgOff = 0;
        int partSize = img.partSize;
        int partOff = 0;
        int splitSize = mtd_info.erasesize;
        while (img.partSize % splitSize)splitSize = splitSize >> 1;
        emxlogd("split size = 0x%x\n", splitSize);
        while (imgSize > 0) {
            uint8_t *tmpbuf = (uint8_t *) img.data;
            int n = imgSize > (int) sizeof(m_buffer) ? (int) sizeof(m_buffer) : imgSize;
            while (imgOff + n > partOff) {
                if (m_flash->Erase(partOff, splitSize) != ErrCodeE::Success) {
                    m_flash->Close();
                    goto ERROR;
                }
                partSize -= splitSize;
                partOff += splitSize;
            }
            //检验是否为末尾数据
            if (imgSize < (int) sizeof(m_buffer)) {
                memset(tmpbuf + imgOff + n, 0xFF, int(sizeof(m_buffer) - n));
                n = (int) sizeof(m_buffer);
            }
            if (m_flash->Write(tmpbuf + imgOff, n) != ErrCodeE::Success) {
                m_flash->Close();
                goto ERROR;
            }
            //移动文件指针位置(向文件开头方向移动n)
            m_flash->Seek(-n, SEEK_CUR);
            if (m_flash->Read(m_buffer, n) != ErrCodeE::Success) {
                emxloge("read failed\n");
                m_flash->Close();
                goto ERROR;
            }
            if (memcmp(m_buffer, tmpbuf + imgOff, n) != 0) {
                emxloge("verify failed\n");
                m_flash->Close();
                goto ERROR;
            }
            imgSize -= n;
            imgOff += n;
            m_event.burn.current += n;
            if (m_event.burn.current >= m_publishSize) {
                PublishEvent();
                m_publishSize += m_event.burn.total / 50;
            }
        }
        if (m_mode == ModeE::Spare && m_systemSpare != SystemSpareE::Spare && img.name == "spare") {
            //如果当前处于spare升级方式，并且运行于Normal分区，则在升级完spare分区的时候需要配置boot从spare启动
            //全部升级完成后会改normal启动
            BootEnv::SetEnv("spare", "1");
            m_systemSpare = SystemSpareE::Spare;
        }
    }
    if (m_romInfo.postScriptSize) {
        emxlogi("execute %s\n", EMX_UPDATE_POST_SCRIPT);
        Cmd::System(EMX_UPDATE_POST_SCRIPT);
    }
    m_event.stat = UpdateEvent::StatE::Done;
    PublishEvent();
    m_bsp.updateIndicator->Stop();
    return ErrCodeE::Success;
    ERROR:
    m_event.stat = UpdateEvent::StatE::Error;
    PublishEvent();
    return ErrCodeE::Failure;
}

ErrCodeE UpdateServer::StreamingBurn() {
    m_publishSize = m_event.burn.total / 50;
    if (m_romInfo.preScriptSize && m_romJson.isRunpreScript) {
        m_romJson.isRunpreScript = false;
        emxlogi("execute %s\n", EMX_UPDATE_PRE_SCRIPT);
        Cmd::System(EMX_UPDATE_PRE_SCRIPT);
    }

    for (auto &img : m_romInfo.img) {
        if (img.imgOff == img.size) {
            emxlogi("burning img [%s] end\n", img.name.c_str());
            continue;
        }
        if (img.imgOff > img.size) {
            emxlogi("img offset[%d] more than img size[%d]\n", img.imgOff, img.size);
            goto ERROR;
        }
        int imgSize = m_burnBlockSize;
        int partSize = img.partSize;
        if ((int)(img.imgOff + m_burnBlockSize) > img.size) {
            // 到达当前img分区末尾了, 取部分数据用于烧写
            imgSize = img.size - img.imgOff;
        }
        int strOff = imgSize;
        emxlogi("burning img [%s] to [%s],imgsize [%d] <-> offser [%d], partsize [%d], burnsize[%d]\n",
                img.name.c_str(), img.dev.c_str(), img.size, img.imgOff, img.partSize, imgSize);
        //如果是spare升级方式，spare img应处于img列表的第一个位置
        if (m_mode == ModeE::Spare && m_systemSpare == SystemSpareE::Spare && img.name == "spare") {
            //如果当前处于spare升级方式，并且运行与spare分区，则不升级spare分区，防止变砖
            m_event.burn.current += imgSize;
            emxlogi("skip spare img\n");
            continue;
        }
        // step3.burning
        if (!img.isOpen) {
            if (m_flash->Open(img.dev.c_str()) != ErrCodeE::Success) {
                goto ERROR;
            }
            img.isOpen = true;
        }
        mtd_info_t mtd_info;
        if (m_flash->GetInfo(mtd_info) != ErrCodeE::Success) {
            goto ERROR;
        }
        emxlogd("mtd erasesize: %d; writesize: %d\n", mtd_info.erasesize, mtd_info.writesize);
        int splitSize = mtd_info.erasesize;
        int imgOff = 0;
        while (img.partSize % splitSize)splitSize = splitSize >> 1;
        emxlogd("split size = 0x%x\n", splitSize);
        uint8_t *endbuf = nullptr; //未数据需要补oxFF操作
        while (imgSize > 0) {
            uint8_t *tmpbuf = (uint8_t *)m_cipherBlock.data.data();
            int n = imgSize > (int) sizeof(m_buffer) ? (int) sizeof(m_buffer) : imgSize;
            int off = n;
            while (img.imgOff + n > img.partOff) {
                if (m_flash->Erase(img.partOff, splitSize) != ErrCodeE::Success) {
                    m_flash->Close();
                    goto ERROR;
                }
                partSize -= splitSize;
                img.partOff += splitSize;
            }
            //检验是否为末尾数据
            if (imgSize < (int) sizeof(m_buffer)) {
                endbuf = (uint8_t*)malloc(m_burnBlockSize);
                memcpy(endbuf, tmpbuf, strOff);
                memset(endbuf + imgOff + n, 0xFF, int(sizeof(m_buffer) - n));
                n = (int) sizeof(m_buffer);
                emxlogd("imgSize: %d\n", imgSize);
            }
            if (endbuf == nullptr) {
                if (m_flash->Write(tmpbuf + imgOff, n) != ErrCodeE::Success) {
                    m_flash->Close();
                    goto ERROR;
                }
            } else {
                if (m_flash->Write(endbuf + imgOff, n) != ErrCodeE::Success) {
                    m_flash->Close();
                    goto ERROR;
                }
            }
            
            //移动文件指针位置(向文件开头方向移动n)
            m_flash->Seek(-n, SEEK_CUR);
            if (m_flash->Read(m_buffer, n) != ErrCodeE::Success) {
                emxloge("read failed\n");
                m_flash->Close();
                goto ERROR;
            }
            if (endbuf == nullptr) {
                if (memcmp(m_buffer, tmpbuf + imgOff, n) != 0) {
                    emxloge("verify failed\n");
                    m_flash->Close();
                    goto ERROR;
                }
            } else {
                if (memcmp(m_buffer, endbuf + imgOff, n) != 0) {
                    emxloge("verify failed\n");
                    m_flash->Close();
                    goto ERROR;
                }
            }
            
            imgSize -= off;
            imgOff += off;
            img.imgOff += off;
            m_event.burn.current += off;
            if (m_event.burn.current >= m_publishSize) {
                PublishEvent();
                m_publishSize += m_event.burn.total / 50;
            }
        }
        if (endbuf != nullptr) {
            free(endbuf);
            endbuf = nullptr;
        }
        if (m_mode == ModeE::Spare && m_systemSpare != SystemSpareE::Spare && img.name == "spare" && img.imgOff >= img.size) {
            //如果当前处于spare升级方式，并且运行于Normal分区，则在升级完spare分区的时候需要配置boot从spare启动
            //全部升级完成后会改normal启动
            emxlogd("spare burn end, set env\n");
            BootEnv::SetEnv("spare", "1");
            m_systemSpare = SystemSpareE::Spare;
        }
        OffsetStr(m_cipherBlock.data, strOff, m_cipherBlock.data.size() - strOff);
        break;
    }
    if (m_cipherBlock.isReachEnd && m_event.burn.current != m_event.burn.total) {
        return ErrCodeE::Success;
    }
    if (m_romInfo.postScriptSize && m_cipherBlock.isReachEnd) {
        emxlogi("execute %s\n", EMX_UPDATE_POST_SCRIPT);
        Cmd::System(EMX_UPDATE_POST_SCRIPT);
    }
    if (m_cipherBlock.isReachEnd) {
        m_event.stat = UpdateEvent::StatE::Done;
        PublishEvent();
        m_bsp.updateIndicator->Stop();
        return ErrCodeE::ReachTheEof;
    }
    return ErrCodeE::Success;
    ERROR:
    m_event.stat = UpdateEvent::StatE::Error;
    PublishEvent();
    return ErrCodeE::Failure;
}

void UpdateServer::SendAck(const struct sockaddr *addr, ErrCodeE e) {
    m_udp.Send((uint8_t *) &e, sizeof(e), addr, sizeof(sockaddr_un));
}

void UpdateServer::UpdateTimer() {
    m_timer.Start(EMX_UPDATE_TIMEOUT, 0, [this]() {
        emxloge("session %d timeout\n", m_session);
        if (m_event.stat == UpdateEvent::StatE::Start) {
            m_event.stat = UpdateEvent::StatE::Error;
            PublishEvent();
        }
        ResetContext();
    });
}

void UpdateServer::ResetContext() {
    m_bsp.updateIndicator->Stop();
    m_timer.Stop();
    m_romJson.isRecved = false;
    m_romJson.isRecvedScript = false;
    m_romJson.isRunpreScript = true;
    m_romJson.size = 0;
    m_cipherBlock.data.clear();
    m_cipherBlock.data.shrink_to_fit();
    m_cipherBlock.isReachEnd = false;
    memset(m_buffer, 0, sizeof(m_buffer));
    memset(&m_fileHeader, 0, sizeof(m_fileHeader));
    m_updating = false;
    m_waitHeader = true;
    m_session = 0;
    m_header.clear();
    m_data.clear();
    m_data.reserve(0);
    m_data.shrink_to_fit();
    m_romInfo.img.clear();
    m_romInfo.preScriptSize = 0;
    m_romInfo.postScriptSize = 0;
    if (m_event.stat != UpdateEvent::StatE::Idle) {
        if (m_rebootAfterComplete) {
            Reboot::DoReboot(m_rebootDelayS);
        }
    }
    memset(&m_event, 0, sizeof(m_event));
}

ErrCodeE UpdateServer::WriteScript(const char *script, const char *data, int size) {
    if (File::Write(script, data, size) != ErrCodeE::Success) {
        remove(script);
        emxloge("write %s script failed\n", script);
        return ErrCodeE::Failure;
    }
    if (chmod(script, S_IRWXU) != 0) {
        remove(script);
        emxloge("chmod %s script failed\n", script);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void UpdateServer::PublishEvent() {
    MsgTopicPublisherSync::Publish(UPDATE_SERVER_MSG_ID,
                                   (const char *) &m_event, sizeof(m_event),
                                   5000);
    emxlogd("stat=%d,session=%d,download[%d/%d],burn=[%d/%d]\n",
            (int) m_event.stat, m_event.session,
            m_event.download.current, m_event.download.total,
            m_event.burn.current, m_event.burn.total);
}

void UpdateServer::TestDecrypt() {
    InitDecrypt();
    std::string str = "helloworldhellohening123456789123456789123456789012345";
    emxlogd("str: %s; size(%d)\n", str.data(), str.size());
    Pkcs7Padding::Pad(str);
    emxlogd("Pad: %s; size(%d)\n", str.data(), str.size());
    m_cipherBlock.aes.Encrypt((uint8_t*)str.data(), str.size());
    emxlogd("Encrypt: %s;size(%d)\n", str.data(), str.size());
    InitDecrypt();
#if 0
    m_cipherBlock.aes.Decrypt((uint8_t*)str.data(), str.size());
    emxlogd("Decrypt: %s;size(%d)\n", str.data(), str.size());
    Pkcs7Padding::UnPad(str);
    emxlogd("UnPad: %s;size(%d)\n", str.data(), str.size());
#else
    int sizeStr = str.size();
    int countStr = sizeStr;
    int off = 0;
    while (countStr) {
        bool isUnpad = false;
        int deSize = 16;
        if ((off + deSize) >= sizeStr) {
            isUnpad = true;
            deSize = sizeStr - off;
        }
        emxlogd("off:size[%d:%d]\n", off, deSize);
        Decrypt((char*)str.data() + off, deSize, isUnpad);
        emxlogd("TestDecrypt: %s\n", str.data());
        countStr -= deSize;
        off += deSize;
        emxlogd("off:countStr[%d:%d]\n", off, countStr);
    }
#endif
    emxlogd("TestDecrypt: %s\n", str.data());
}

void UpdateServer::TestOffsetStr() {
    std::string str = "helloworldhellohening123456789123456789123456789012345";
    emxlogd("str: %s; size(%d)\n", str.data(), str.size());
    int off = 16;
    OffsetStr(str, off, str.size() - off);
    emxlogd("str: %s; size(%d)\n", str.data(), str.size());
}