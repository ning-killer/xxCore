/*
* @Author: xiong
* @Date: 2022/8/15.
*/

#ifndef EAPIL_Record_HPP
#define EAPIL_Record_HPP

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "SDCardClient.hpp"
#include "Mp4.hpp"
#include "OvdCtx.hpp"

namespace Emx {

    class Record {
    public:
        Record() : m_created(false), m_paramChange(false), m_recording(false) {}

        struct Config {
            char path[EMX_MAX_PATH_SIZE];
            uint32_t splitTimeMs;
            uint32_t reservedSpace;//!<保留空间，单位MB
            int vencChn;
            int aencChn;
        };

        ErrCodeE Create(OvdCtx *ctx);

        void Destroy();

        bool IsCreated() { return m_created; }

        struct RecFile {
            uint32_t startUtc;
            uint32_t endUtc;
            int32_t size;
            int32_t date;
            int32_t time;
        };

        void GetFileIndex(uint32_t startUtc, uint32_t endUtc,
                          std::vector<RecFile> &index, bool merge = false);

        ErrCodeE Seek(uint32_t utc, RecFile &file, char *path);

        char *GetFilePath(RecFile &file, char *path);

    private:

        //!@brief 创建录像文件索引，顺便删除空目录
        ErrCodeE CreateIndex();

        void DestroyIndex();

        ErrCodeE FreeSpace();

        void OnSDCardInfoEvent(int chn, SDCard::Info &info);

        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

        bool IsEmptyDirector(const char *path);

        void CalibrationFilename();

        ErrCodeE CreateNewFile(uint32_t startUtc);

        ErrCodeE CompleteFile(uint32_t endUtc);

        ErrCodeE TryCreateDir(const char *path);

        void GetAVInfo();

    private:
        Config m_cfg;
        bool m_created;
        std::mutex m_mutex;
        EuvLoop m_loop;
        EuvTimer m_timer;
        bool m_splitTimeout;
        bool m_paramChange;
        bool m_recording;
        MediaClientStreamAsync *m_stream;
        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
        // MediaRW::UserInfo m_userInfo;
        Mp4Writer *m_mp4Writer;
        char m_buffer[256];
        std::map<uint32_t, std::map<uint32_t, std::list<RecFile>>> m_index;
        using TrackPair = std::pair<MediaFrame::TypeE, int>;
        std::map<TrackPair, int> m_trackIdxMap;
        RecFile m_current;
        struct SDCardStat {
            bool normal;
            int chn;
            SDCardClientEvent *event;
        } m_sdcard;
    };

}

#endif //EAPIL_Record_HPP
