/*
* @Author: xiong
* @Date: 2023/2/28
*/

#include "RecordStdRecordFile.hpp"
#include "EmxMedia.hpp"
#include "SDCardClient.hpp"

using namespace Emx;


ErrCodeE RecordStdRecordFile::CreateIndex(const char *top) {
    TimeRec cost;
    if (TryCreateDir(top) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    snprintf(m_recordDir, sizeof(m_recordDir), "%s/record", top);
    if (TryCreateDir(m_recordDir) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    DIR *topDir = opendir(m_recordDir);
    if (!topDir) {
        emxloge("cannot open directory: %s\n ", m_recordDir);
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
    struct dirent *dateEntry;
    while ((dateEntry = readdir(topDir)) != nullptr) {
//        printf("%s:%s:%d:dateEntry->d_name=%s\n", __FILE__, __FUNCTION__, __LINE__, dateEntry->d_name);
        if (dateEntry->d_type != DT_DIR || !IsNumber(dateEntry->d_name))
            continue;
        char datePath[EMX_MAX_PATH_SIZE] = {};
        snprintf(datePath, sizeof(datePath), "%s/%s", m_recordDir, dateEntry->d_name);
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
                RecordStd::RecFile file = {};
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
//    m_mutex.lock();
    for (auto &date:m_index) {
        for (auto &hour:date.second) {
            hour.second.sort([](RecordStd::RecFile &first, RecordStd::RecFile &second) {
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

void RecordStdRecordFile::DestroyIndex() {
    m_index.clear();
}

//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
ErrCodeE RecordStdRecordFile::CreateNewFile(uint32_t startUtc, int vencChn, int aencChn) {
    memset(&m_current, 0, sizeof(m_current));
    m_current.startUtc = startUtc;
    m_current.size = -1;
    struct tm tm = {};
    localtime_r((time_t *) &startUtc, &tm);
    m_current.date = (tm.tm_year + 1900) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;
    m_current.time = tm.tm_hour * 10000 + tm.tm_min * 100 + tm.tm_sec;
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/%08d", m_recordDir, m_current.date);
    if (TryCreateDir(path) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    snprintf(path, sizeof(path), "%s/%08d/%02d", m_recordDir, m_current.date, m_current.time / 10000);
    if (TryCreateDir(path) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    snprintf(path, sizeof(path), "%s/%08d/%02d/%u.tmp", m_recordDir, m_current.date, m_current.time / 10000, startUtc);
//    m_mp4Writer = new FlvWriter;
    m_mp4Writer = new Mp4Writer;
//    m_mp4Writer = new Fmp4Writer;
    if (m_mp4Writer->Create(path, nullptr) != ErrCodeE::Success) {
        delete m_mp4Writer;
        m_mp4Writer = nullptr;
        return ErrCodeE::Failure;
    }
    GetAVInfo(vencChn, aencChn);

    emxlogd("%s created\n", path);
    return ErrCodeE::Success;
}

//path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
ErrCodeE RecordStdRecordFile::CompleteFile(uint32_t endUtc) {
    if (m_mp4Writer) {
        m_mp4Writer->Destroy();
        delete m_mp4Writer;
        m_mp4Writer = nullptr;
        m_current.endUtc = endUtc;
        char oldName[EMX_MAX_PATH_SIZE] = {};
        snprintf(oldName, sizeof(oldName), "%s/%08d/%02d/%u.tmp",
                 m_recordDir, m_current.date, m_current.time / 10000, m_current.startUtc);
        char newName[EMX_MAX_PATH_SIZE] = {};
        snprintf(newName, sizeof(newName), "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                 m_recordDir,
                 m_current.date, m_current.time / 10000,
                 m_current.date, m_current.time,
                 m_current.startUtc, m_current.endUtc);
        rename(oldName, newName);
        m_current.size = File::Size(newName);
        m_index[m_current.date][m_current.time / 10000].push_back(m_current);
        emxlogt("%s completed\n", newName);
    }
    return ErrCodeE::Success;
}

ErrCodeE RecordStdRecordFile::FreeSpace(int sdcardChn, int reservedSpace) {
    TimeRec cost;
    SDCard::Info info = {};
    SDCardClient sdcard(sdcardChn);
    if (sdcard.GetInfo(info) != ErrCodeE::Success) {
        emxloge("get sdcard %d info failed, cost = %ums\n", sdcardChn, cost.GetInv());
        return ErrCodeE::ResNotAvailable;
    }
    if ((int) info.free > reservedSpace) {
        return ErrCodeE::Success;
    }

    //用于记录被删除的文件所属的hour和date目录，便于后续删除空目录
    std::map<int32_t, std::map<uint8_t, bool>> delDir;
    //这里为了遍历的过程中安全的删除链表中的元素，++操作不能随意修改
    int deleteCnt = 0;
    char tmp[EMX_MAX_PATH_SIZE] = {};
    for (auto date = m_index.begin(); date != m_index.end() && !deleteCnt;) {
        for (auto hour = date->second.begin(); hour != date->second.end() && !deleteCnt;) {
            //尝试删除小时目录下的所有文件
            for (auto file = hour->second.begin(); file != hour->second.end();) {
                //path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
                snprintf(tmp, sizeof(tmp), "%s/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                         m_recordDir, file->date, file->time / 10000,
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
    //删除空目录
    for (auto date = delDir.begin(); date != delDir.end();) {
        for (auto hour = date->second.begin(); hour != date->second.end();) {
            //path/20181128/09/VID_20181128_092642(1543368402_1543368403).mp4
            snprintf(tmp, sizeof(tmp), "%s/%08d/%02d", m_recordDir, date->first, hour->first);
            if (IsEmptyDirector(tmp)) {
                emxlogd("remove dir %s\n", tmp);
                rmdir(tmp);
            }
        }
        snprintf(tmp, sizeof(tmp), "%s/%08d", m_recordDir, date->first);
        if (IsEmptyDirector(tmp)) {
            emxlogd("remove dir %s\n", tmp);
            rmdir(tmp);
        }
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

ErrCodeE RecordStdRecordFile::TryCreateDir(const char *path) {
    if (!File::Exist(path)) {
        if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            emxloge("create dir %s failed with %s\n", path, strerror(errno));
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}

bool RecordStdRecordFile::IsEmptyDirector(const char *path) {
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

void RecordStdRecordFile::GetAVInfo(int vencChn, int aencChn) {
    m_mediaInfo.clear();
    m_trackIdxMap.clear();
    if (vencChn >= 0) {
        MediaVenc::Param param = {};
        MediaClientVenc venc(vencChn);
        if (venc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Video;
            auto &video = info.info.video;
            video.codec = param.codec;
            video.width = param.width;
            video.height = param.height;
            TrackPair pair(MediaFrame::TypeE::Venc, vencChn);
            m_trackIdxMap[pair] = m_mp4Writer->AddTrack(info);
            m_mediaInfo.push_back(info);
        } else {
            emxloge("venc %d get param failed\n", vencChn);
        }
    }
    if (aencChn >= 0) {
        MediaAenc::Param param = {};
        MediaClientAenc aenc(aencChn);
        if (aenc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Audio;
            auto &audio = info.info.audio;
            audio.codec = param.codec;
            audio.sampleRate = param.sampleRate;
            audio.bitWidth = param.bitWidth;
            audio.channel = 1;
            TrackPair pair(MediaFrame::TypeE::Aenc, aencChn);
            m_trackIdxMap[pair] = m_mp4Writer->AddTrack(info);
            m_mediaInfo.push_back(info);
        } else {
            emxloge("aenc %d get param failed\n", aencChn);
        }
    }
}

ErrCodeE RecordStdRecordFile::WriteFile(MediaFrame &frame) {
    if (m_mp4Writer) {
        MediaFmt::Frame f = {};
        if (frame.type == MediaFrame::TypeE::Venc) {
            f.type = MediaFmt::TrackTypeE::Video;
        } else if (frame.type == MediaFrame::TypeE::Aenc) {
            f.type = MediaFmt::TrackTypeE::Audio;
        } else {
            emxloge("not support frame type %d\n", (int) frame.type);
            return ErrCodeE::IllegalParam;
        }
        TrackPair pair(frame.type, frame.chn);
        if (!m_trackIdxMap.count(pair)) {
            emxloge("not register frame type %d with chn %d\n", (int) frame.type, frame.chn);
            return ErrCodeE::IllegalParam;
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
        m_mp4Writer->Write(f);
    }
    return ErrCodeE::Success;
}

void RecordStdRecordFile::GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge,
                                        std::vector<RecordStd::RecFile> &list) {
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
                if (merge && !list.empty()) {
                    auto &back = list.back();
                    if (back.endUtc == file.startUtc) {
                        back.endUtc = file.endUtc;
                        back.size += file.size;
                        continue;
                    }
                }
                list.emplace_back(file);
            }
        }
    }
}
