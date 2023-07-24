/*
* @Author: xiong
* @Date: 2022/8/15.
*/

#include "Record.hpp"
#include "OvdUtils.hpp"
#include "Mac.hpp"
#include <sys/stat.h>
#include <algorithm>

using namespace Emx;

ErrCodeE Record::Create(OvdCtx *ctx) {
    if (m_created)
        return ErrCodeE::Success;
    auto &record = ctx->deviceJsonCfg["record"];
    // strncpy(m_cfg.path, record["path"].asCString(), sizeof(m_cfg.path));
    m_cfg.splitTimeMs = record["splitTimeMs"].asInt();
    m_cfg.reservedSpace = record["reservedSpace"].asUInt();
    m_cfg.vencChn = record["vencChn"].asInt();//-1:means no video
    m_cfg.aencChn = record["aencChn"].asInt();//-1:means no audio
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        emxloge("record load mac failed!\n");
        return ErrCodeE::Failure;
    }
    std::string s_mac(mac.Get(), strlen(mac.Get()));
    s_mac.erase(std::remove(s_mac.begin(), s_mac.end(), ':'), s_mac.end());
    memset(m_cfg.path, 0, sizeof(m_cfg.path));
    snprintf(m_cfg.path
            , sizeof(m_cfg.path) - 1
            , "%s/xxxx_%s"
            , record["path"].asString().c_str()
            , s_mac.c_str());

    m_loop.Init("Record", [this]() {
        CompleteFile(Time::GetS());
        DestroyIndex();
        m_stream->Stop();
        delete m_stream;
        m_sdcard.event->Destroy();
        delete m_sdcard.event;
        m_timer.Destroy();
    });

    //先将sdcard的状态配置为无效
    m_sdcard.normal = false;
    //创建SDCardClientEvent来监听sd的状态改变事件
    m_sdcard.event = new SDCardClientEvent(m_loop,
                                           m_buffer, sizeof(m_buffer),
                                           std::bind(&Record::OnSDCardInfoEvent, this, ph_1, ph_2));
    // sdcard event 创建成功后会收到OnSDCardInfoEvent的异步回调,由此开始循环创建录像文件
    m_sdcard.event->Create();

    //启动接收流媒体数据
    MediaClientStream::Channel chn;
    if (m_cfg.vencChn >= 0)
        chn.Add(MediaFrame::TypeE::Venc, m_cfg.vencChn);
    if (m_cfg.aencChn >= 0)
        chn.Add(MediaFrame::TypeE::Aenc, m_cfg.aencChn);
    m_stream = new MediaClientStreamAsync(m_loop);
    m_stream->Start(chn, std::bind(&Record::OnGetFrame, this, ph_1, ph_2));

    //创建定时器，先不启动，创建新的文件后启动
    m_timer.Create(m_loop);

    m_loop.Start();
    m_created = true;
    emxlogd("Record created path: %s\n", m_cfg.path);
    return ErrCodeE::Success;
}

void Record::Destroy() {
    if (m_created) {
        m_loop.StopAndDeInit();
        m_created = false;
        emxlogi("Record destroyed\n");
    }
}

//校正存储文件夹名称
void Record::CalibrationFilename() {
    char old_path[64] = { 0 };
    char new_path[64] = { 0 };
    snprintf(old_path, sizeof(old_path) - 1, "/mnt/sdcard/record");
    snprintf(new_path, sizeof(new_path) - 1, "%s", m_cfg.path);
    struct stat st;
    if (stat(old_path, &st) != 0) {
        return;
    }
    rename(old_path, new_path);
}

//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
void Record::GetFileIndex(uint32_t startUtc, uint32_t endUtc,
                          std::vector<RecFile> &index, bool merge) {
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    for (auto &date : m_index) {
        //如果当前日期下的最后一个小时的最后一个录像文件的结束时间小于请求区间的起始时间，则跳过此天
        if ((--date.second.end())->second.back().endUtc <= startUtc)
            continue;
        for (auto &hour : date.second) {
            //如果当前小时下的最后一个录像文件的结束时间大于请求区间的起始时间，则跳过此小时
            if (hour.second.back().endUtc <= startUtc)
                continue;
            for (auto &file :hour.second) {
                //过滤掉起始时间前的录像(UTC)
                if (file.endUtc <= startUtc)
                    continue;
                //仅使用此处的UTC时间判断来作为退出条件即可
                if (file.startUtc >= endUtc)
                    return;
                //合并连续的时间段
                if (merge && !index.empty()) {
                    auto &back = index.back();
                    if (back.endUtc == file.startUtc) {
                        back.endUtc = file.endUtc;
                        back.size += file.size;
                        continue;
                    }
                }
                index.emplace_back(file);
            }
        }
    }
}

ErrCodeE Record::Seek(uint32_t utc, RecFile &file, char *path) {
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    for (auto &date : m_index) {
        //如果当前日期下的最后一个小时的最后一个录像文件的结束时间小于请求区间的起始时间，则跳过此天
        if ((--date.second.end())->second.back().endUtc <= utc)
            continue;
        for (auto &hour : date.second) {
            //如果当前小时下的最后一个录像文件的结束时间大于请求区间的起始时间，则跳过此小时
            if (hour.second.back().endUtc <= utc)
                continue;
            for (auto &f :hour.second) {
                //过滤掉起始时间前的录像(UTC)
                if (f.endUtc <= utc)
                    continue;
                file = f;
                snprintf(path, EMX_MAX_PATH_SIZE, "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                         m_cfg.path, file.date, file.time / 10000,
                         file.date, file.time, file.startUtc, file.endUtc);
                return ErrCodeE::Success;
            }
        }
    }
    return ErrCodeE::ReachTheEof;
}

char *Record::GetFilePath(Record::RecFile &file, char *path) {
    snprintf(path, EMX_MAX_PATH_SIZE, "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
             m_cfg.path, file.date, file.time / 10000,
             file.date, file.time, file.startUtc, file.endUtc);
    return path;
}

void Record::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (!m_recording)
        return;
    if (e != ErrCodeE::Success)
        return;

    //设想中venc的frame.paramChange和frame.isKeyFrame应该同时出现，这里加打印看看会不会有异常情况
    if (frame.type == MediaFrame::TypeE::Venc && frame.paramChange && !frame.isKeyFrame)
        emxlogc("chn %d venc param changed but is not key frame\n", frame.chn);

    bool createNewFile = false;
    //参数变化了需要等待I帧重新录制文件，期间的所有数据丢弃
    if (frame.paramChange)
        m_paramChange = true;
    if (m_paramChange) {
        if (frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
            m_paramChange = false;
            createNewFile = true;
        } else {
            return;
        }
    }
    if (m_splitTimeout && frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
        createNewFile = true;
        m_splitTimeout = false;
    }
    if (createNewFile) {
        uint32_t curUtc = Time::GetS();
        CompleteFile(curUtc);
        if (FreeSpace() != ErrCodeE::Success) {
            emxloge("free space failed\n");
            return;
        }
        CreateNewFile(curUtc);
    }

    if (m_mp4Writer) {
        MediaFmt::Frame f = {};
        if (frame.type == MediaFrame::TypeE::Venc) {
            f.type = MediaFmt::TrackTypeE::Video;
            //note: video 加密(数据长度大于256加密帧头128长度数据)
            if (frame.size > 256 && frame.data != nullptr) {
                if (!OvdUtils::EncryDecryMp4Video(frame.data, 128, frame.isKeyFrame)) {
                    emxloge("EncryDecryMp4Video failed\n");
                }
            }
        } else if (frame.type == MediaFrame::TypeE::Aenc) {
            f.type = MediaFmt::TrackTypeE::Audio;
            //note: audio 加密(数据帧前7个字节不做加密操作)
            //note: 暂不加密
            //OvdUtils::EncryDecryMp4Audio(frame.data + 7, frame.size - 7);
        } else {
            emxloge("not support frame type %d\n", (int) frame.type);
            return;
        }
        TrackPair pair(frame.type, frame.chn);
        if (!m_trackIdxMap.count(pair)) {
            emxloge("not register frame type %d with chn %d\n", (int) frame.type, frame.chn);
            return;
        }
        f.trackId = m_trackIdxMap[pair];
        f.isKeyFrame = frame.isKeyFrame;
        f.data = frame.data;
        f.size = frame.size;
        f.pts = frame.tsInUs / 1000;
//        printf("%s:%d write[%d,%d,%d,%llu,%d]\n",
//                __FUNCTION__, __LINE__,
//               (int) f.type, f.isKeyFrame, f.size, f.pts, f.trackId);
//        for (int i = 0; i < 10; i++) {
//            printf("%02x ", f.data[i]);
//        }
//        printf("\n");
        if (m_mp4Writer->Write(f) != ErrCodeE::Success) {
            emxloge("mp4 write failed!\n");
        }
    }
}

void Record::OnSDCardInfoEvent(int chn, SDCard::Info &info) {
    //校正存储文件夹名称
    CalibrationFilename();
    //确认存储路径所在SD卡通道号
    if (strncmp(info.mountPoint, m_cfg.path, strlen(info.mountPoint)) == 0) {
        m_sdcard.chn = chn;
    } else {
        return;
    }
    bool normal = info.stat == SDCard::StatE::Normal;
    if (m_sdcard.normal == normal)
        return;
    m_sdcard.normal = normal;
    emxlogi("stat normal = %s\n", normal ? "true" : "false");
    if (normal) {
        if (CreateIndex() == ErrCodeE::Success) {
            if (CreateNewFile(Time::GetS()) == ErrCodeE::Success) {
                m_recording = true;
            }
        }
    } else {
        m_recording = false;
        if (m_mp4Writer) {
            m_mp4Writer->Destroy();
            delete m_mp4Writer;
            m_mp4Writer = nullptr;
        }
        DestroyIndex();
        m_timer.Stop();
    }
}

//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
ErrCodeE Record::CreateNewFile(uint32_t startUtc) {
    memset(&m_current, 0, sizeof(m_current));
    m_current.startUtc = startUtc;
    m_current.size = -1;
    struct tm tm = {};
    localtime_r((time_t *) &startUtc, &tm);
    m_current.date = (tm.tm_year + 1900) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;
    m_current.time = tm.tm_hour * 10000 + tm.tm_min * 100 + tm.tm_sec;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/%08d", m_cfg.path, m_current.date);
    if (TryCreateDir(path) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    snprintf(path, sizeof(path), "%s/%08d/%02d", m_cfg.path, m_current.date, m_current.time / 10000);
    if (TryCreateDir(path) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    snprintf(path, sizeof(path), "%s/%08d/%02d/%u.tmp",
             m_cfg.path, m_current.date, m_current.time / 10000, startUtc);
    m_mp4Writer = new Mp4Writer;
    if (m_mp4Writer->Create(path, nullptr) != ErrCodeE::Success) {
        delete m_mp4Writer;
        m_mp4Writer = nullptr;
        return ErrCodeE::Failure;
    }
    GetAVInfo();
    //开启定时器
    m_timer.Start(m_cfg.splitTimeMs, 0, [this]() { m_splitTimeout = true; });
    emxlogd("%s created\n", path);
    return ErrCodeE::Success;
}

//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
ErrCodeE Record::CompleteFile(uint32_t endUtc) {
    if (m_mp4Writer) {
        m_mp4Writer->Destroy();
        delete m_mp4Writer;
        m_mp4Writer = nullptr;
        m_current.endUtc = endUtc;
        char oldName[EMX_MAX_PATH_SIZE] = {};
        snprintf(oldName, sizeof(oldName), "%s/%08d/%02d/%u.tmp",
                 m_cfg.path, m_current.date, m_current.time / 10000, m_current.startUtc);
        char newName[EMX_MAX_PATH_SIZE] = {};
        snprintf(newName, sizeof(newName), "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                 m_cfg.path,
                 m_current.date, m_current.time / 10000,
                 m_current.date, m_current.time,
                 m_current.startUtc, m_current.endUtc);
        rename(oldName, newName);
        m_current.size = File::Size(newName);
        m_mutex.lock();
        m_index[m_current.date][m_current.time / 10000].push_back(m_current);
        m_mutex.unlock();
        emxlogt("%s completed\n", newName);
    }
    return ErrCodeE::Success;
}

ErrCodeE Record::TryCreateDir(const char *path) {
    if (!File::Exist(path)) {
        if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            emxloge("create dir %s failed with %s\n", path, strerror(errno));
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}

//假设设备的时区会变化
//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
//path/date/hour/VID_date_time(startUtc_endUtc).suffix
ErrCodeE Record::CreateIndex() {
    TimeRec cost;
    if (TryCreateDir(m_cfg.path) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    DIR *topDir = opendir(m_cfg.path);
    if (!topDir) {
        emxloge("cannot open directory: %s\n ", m_cfg.path);
        return ErrCodeE::Failure;
    }
    //lambda:name is pure digit
    auto IsNumber = [&](const char *number) {
        for (int i = 0; i < (int) strlen(number); i++)
            if (!isdigit(number[i]))
                return false;
        return true;
    };
    //top dir scan date
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    struct dirent *dateEntry;
    while ((dateEntry = readdir(topDir)) != nullptr) {
//        printf("%s:%s:%d:dateEntry->d_name=%s\n", __FILE__, __FUNCTION__, __LINE__, dateEntry->d_name);
        if (dateEntry->d_type != DT_DIR || !IsNumber(dateEntry->d_name))
            continue;
        char datePath[EMX_MAX_PATH_SIZE] = {};
        snprintf(datePath, sizeof(datePath), "%s/%s", m_cfg.path, dateEntry->d_name);
        DIR *dateDir = opendir(datePath);
        if (!dateDir) {
            emxloge("cannot open directory: %s\n ", datePath);
            continue;
        }
        //date dir scan hour
        int hourCnt = 0;
        struct dirent *hourEntry;
        while ((hourEntry = readdir(dateDir)) != nullptr) {
//            printf("%s:%s:%d:hourEntry->d_name=%s\n", __FILE__, __FUNCTION__, __LINE__, hourEntry->d_name);
            if (hourEntry->d_type != DT_DIR || !IsNumber(hourEntry->d_name))
                continue;
            char hourPath[EMX_MAX_PATH_SIZE] = {};
            snprintf(hourPath, sizeof(hourPath), "%s/%s", datePath, hourEntry->d_name);
            DIR *hourDir = opendir(hourPath);
            if (!hourDir) {
                emxloge("cannot open directory: %s\n ", hourPath);
                continue;
            }
            //hour dir scan file
            struct dirent *fileEntry;
            int fileCnt = 0;
            while ((fileEntry = readdir(hourDir)) != nullptr) {
//                printf("%s:%s:%d:fileEntry->d_name=%s\n", __FILE__, __FUNCTION__, __LINE__, fileEntry->d_name);
                RecFile file = {};
                int date;
                int time;
                //path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
                //path/date/hour/VID_date_time(startUtc_endUtc).suffix
                if (strstr(fileEntry->d_name, ".tmp")) {
                    char filePath[EMX_MAX_PATH_SIZE] = {};
                    snprintf(filePath, sizeof(filePath), "%s/%s", hourPath, fileEntry->d_name);
                    remove(filePath);
                    continue;
                }
                int res = sscanf(fileEntry->d_name, "VID_%8d_%6d(%u_%u).mp4",
                                 &date, &time, &file.startUtc, &file.endUtc);
                if (res != 4)
                    continue;
                file.date = date;
                file.time = time;
                char filePath[EMX_MAX_PATH_SIZE] = {};
                snprintf(filePath, sizeof(filePath), "%s/%s", hourPath, fileEntry->d_name);
                file.size = File::Size(filePath);
                uint32_t hourUtc = file.startUtc / 60 / 60;
                uint32_t dateUtc = hourUtc / 24;
//                printf("m_index[%u][%u].push_back(%u-%u)\n", dateUtc, hourUtc, file.startUtc, file.endUtc);
                m_index[dateUtc][hourUtc].push_back(file);
                fileCnt++;
            }
            closedir(hourDir);
            if (fileCnt)
                hourCnt++;
            else
                remove(hourPath);
        }
        closedir(dateDir);
        if (!hourCnt)
            remove(datePath);
    }
    closedir(topDir);
    int fileCount = 0;
    for (auto &date:m_index) {
        for (auto &hour:date.second) {
            hour.second.sort([](RecFile &first, RecFile &second) {
                return first.startUtc < second.startUtc;
            });
            fileCount += (int) hour.second.size();
        }
    }
//    for (auto &date:m_index)
//        for (auto &hour:date.second)
//            for (auto &file:hour.second) {
//                printf("date:%u,hour:%u,file[%u-%u]\n", date.first, hour.first, file.startUtc, file.endUtc);
//            }
    emxlogi("file count %d cost = %ums\n", fileCount, cost.GetInv());
    return ErrCodeE::Success;
}

void Record::DestroyIndex() {
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_index.clear();
}

ErrCodeE Record::FreeSpace() {
    TimeRec cost;
    SDCardClient sdcard(m_sdcard.chn);
    SDCard::Info info = {};
    if (sdcard.GetInfo(info) != ErrCodeE::Success) {
        emxloge("get sd info failed\n");
        return ErrCodeE::Failure;
    }
    if (info.stat != SDCard::StatE::Normal) {
        emxloge("sdcard stat = %d\n", info.stat);
        return ErrCodeE::Failure;
    }
    if (info.free > m_cfg.reservedSpace)
        return ErrCodeE::Success;
    //用于记录被删除的文件所属的hour和date目录，便于后续删除空目录
    std::map<int32_t, std::map<uint8_t, bool>> delDir;
    //这里为了遍历的过程中安全的删除链表中的元素，++操作不能随意修改
    int deleteCnt = 0;
    char tmp[EMX_MAX_PATH_SIZE] = {};
    m_mutex.lock();
    for (auto date = m_index.begin(); date != m_index.end() && !deleteCnt;) {
        for (auto hour = date->second.begin(); hour != date->second.end() && !deleteCnt;) {
            //尝试删除小时目录下的所有文件
            for (auto file = hour->second.begin(); file != hour->second.end();) {
                //path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
                snprintf(tmp, sizeof(tmp), "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                         m_cfg.path, file->date, file->time / 10000,
                         file->date, file->time, file->startUtc, file->endUtc);
                if (remove(tmp) == 0) {
                    //记录下哪些文件夹下存在删除操作，为后续删除文件夹做准备
                    delDir[file->date][file->time / 10000] = true;
                    file = hour->second.erase(file);
                    deleteCnt++;
                } else {
                    emxloge("remove %s failed with %s\n", tmp, strerror(errno));
                    file++;
                }
            }
            //如果小时已经空了，则删除
            if (hour->second.empty()) {
                hour = date->second.erase(hour);
            } else {
                hour++;
            }
        }
        //如果日期已经空了，则删除
        if (date->second.empty()) {
            date = m_index.erase(date);
        } else {
            date++;
        }
    }
    m_mutex.unlock();
    //删除空目录
    for (auto date = delDir.begin(); date != delDir.end();) {
        for (auto hour = date->second.begin(); hour != date->second.end();) {
            //path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
            snprintf(tmp, sizeof(tmp), "%s/%08d/%02d", m_cfg.path, date->first, hour->first);
            if (IsEmptyDirector(tmp)) {
                emxlogd("remove dir %s\n", tmp);
                rmdir(tmp);
            }
            hour++;
        }
        snprintf(tmp, sizeof(tmp), "%s/%08d", m_cfg.path, date->first);
        if (IsEmptyDirector(tmp)) {
            emxlogd("remove dir %s\n", tmp);
            rmdir(tmp);
        }
        date++;
    }
    if (deleteCnt) {
        //如果至少成功删除了一个文件，那么认为此次free space操作是成功的
        emxlogd("deleted %d file,cost = %ums\n", deleteCnt, cost.GetInv());
        return ErrCodeE::Success;
    } else {
        //SD卡的空间不足，但是一个文件也没有被成功删除
        emxlogc("free space failed, cost = %ums\n", cost.GetInv());
        return ErrCodeE::Failure;
    }
}

bool Record::IsEmptyDirector(const char *path) {
    DIR *dp = opendir(path);
    if (!dp)
        return false;
    int num = 0;
    while (readdir(dp)) {
        num++;
        if (num > 2)
            break;
    }
    closedir(dp);
    return num == 2;
}

void Record::GetAVInfo() {
    m_mediaInfo.clear();
    m_trackIdxMap.clear();
    if (m_cfg.vencChn >= 0) {
        MediaVenc::Param param = {};
        MediaClientVenc venc(m_cfg.vencChn);
        if (venc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Video;
            auto &video = info.info.video;
            video.codec = param.codec;
            video.width = param.width;
            video.height = param.height;
            TrackPair pair(MediaFrame::TypeE::Venc, m_cfg.vencChn);
            m_trackIdxMap[pair] = m_mp4Writer->AddTrack(info);
            m_mediaInfo.push_back(info);
        } else {
            emxloge("venc %d get param failed\n", m_cfg.vencChn);
        }
    }
    if (m_cfg.aencChn >= 0) {
        MediaAenc::Param param = {};
        MediaClientAenc aenc(m_cfg.aencChn);
        if (aenc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Audio;
            auto &audio = info.info.audio;
            audio.codec = param.codec;
            audio.sampleRate = param.sampleRate;
            audio.bitWidth = param.bitWidth;
            audio.channel = 1;
            TrackPair pair(MediaFrame::TypeE::Aenc, m_cfg.aencChn);
            m_trackIdxMap[pair] = m_mp4Writer->AddTrack(info);
            m_mediaInfo.push_back(info);
        } else {
            emxloge("aenc %d get param failed\n", m_cfg.aencChn);
        }
    }
}
