/*
* @Author: xiong
* @Date: 2023/3/1
*/

#ifndef EMX_RecordStdSnapFile_HPP
#define EMX_RecordStdSnapFile_HPP

#include "RecordStdResource.hpp"
#include "EmxMedia.hpp"

namespace Emx {

    class RecordStdSnapFile {
    public:
        RecordStdSnapFile() {}

        ErrCodeE CreateIndex(const char *top);

        void DestroyIndex();

        ErrCodeE WriteFile(uint32_t utc, MediaFrame &frame);

        ErrCodeE FreeSpace(int sdcardChn, int reservedSpace);

        void GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector<RecordStd::RecFile> &list);

    private:

        static ErrCodeE TryCreateDir(const char *path);

        static bool IsEmptyDirector(const char *path);

    protected:
        char m_snapDir[EMX_MAX_PATH_SIZE];
        std::map<uint32_t, std::map<uint32_t, std::list<RecordStd::RecFile>>> m_index;
    };

}

#endif //EMX_RecordStdSnapFile_HPP
