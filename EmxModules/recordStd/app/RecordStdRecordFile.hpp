/*
* @Author: xiong
* @Date: 2023/2/28
*/

#ifndef EMX_RecordStdRecordFile_HPP
#define EMX_RecordStdRecordFile_HPP

#include "RecordStdResource.hpp"
#include "Mp4.hpp"
//#include "Fmp4.hpp"

namespace Emx {

    class RecordStdRecordFile {
    public:
        RecordStdRecordFile() {
            m_mp4Writer = nullptr;
        }

        ErrCodeE CreateIndex(const char *top);

        void DestroyIndex();

        ErrCodeE CreateNewFile(uint32_t startUtc, int vencChn, int aencChn);

        ErrCodeE WriteFile(MediaFrame &frame);

        ErrCodeE CompleteFile(uint32_t endUtc);

        ErrCodeE FreeSpace(int sdcardChn, int reservedSpace);

        void GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge, std::vector<RecordStd::RecFile> &list);

    private:

        static ErrCodeE TryCreateDir(const char *path);

        static bool IsEmptyDirector(const char *path);

        void GetAVInfo(int vencChn, int aencChn);


    protected:
        char m_recordDir[EMX_MAX_PATH_SIZE];
        std::map<uint32_t, std::map<uint32_t, std::list<RecordStd::RecFile>>> m_index;
        RecordStd::RecFile m_current;
//        Fmp4Writer *m_mp4Writer;
        Mp4Writer *m_mp4Writer;
//        FlvWriter *m_mp4Writer;
        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
        using TrackPair = std::pair<MediaFrame::TypeE, int>;
        std::map<TrackPair, int> m_trackIdxMap;
    };
}
#endif //EMX_RecordStdRecordFile_HPP
