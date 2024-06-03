#include "EmxCore.hpp"
#include "Mp4.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

class DemoMp4 {
    public:
        DemoMp4() {};
        ~DemoMp4() {};

    bool CreateReader(const char* path) {
        if (m_mp4Reader == nullptr) {
            m_mp4Reader = new Mp4Reader();
            emxlogd("m_mp4Reader:%p\n", m_mp4Reader);
        }
        std::vector<MediaFmt::TrackInfo> mediaInfo;
        auto e = m_mp4Reader->Create(path, mediaInfo, nullptr);
        if (e != ErrCodeE::Success) {
            emxloge("create mp4 reader failed\n");
            return false;
        }
        trackMap.clear();
        for (auto &item : mediaInfo) {
            trackMap[item.trackId] = item;
        }
        emxlogd("create mp4 reader ok\n");
        return true;
    }

    void Seek(int64_t &pos) {
        if (m_mp4Reader != nullptr) {
            m_mp4Reader->Seek(pos);
        }
    }

    private:
        Mp4Reader *m_mp4Reader = nullptr;
        std::map<int, MediaFmt::TrackInfo> trackMap;
        Mp4Writer *m_mp4Writer = nullptr;
};


int main(int argc, char *argv[]) {
    DemoMp4 mp4;
    std::string path = "/mnt/sdcard/xxxx_0CF0B40A4338/20240222/18/VID_20240222_185345(1708599225_1708599287).mp4";
    mp4.CreateReader(path.c_str());
    for (int i = 0; i <= 60; i++) {
        // int64_t pos = (timestamp - file.startUtc) * 1000;
        int64_t pos = i * 1000;
        emxlogd("input pos: %d\n", pos);
        mp4.Seek(pos);
        emxlogd("ouput seek pts: %d\n", pos);
    }
    return 0;
}