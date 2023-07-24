/*
* @Author: xiong
* @Date: 2023/3/7
*/

#ifndef EMX_ReadMp4_HPP
#define EMX_ReadMp4_HPP

#include "Fmp4.hpp"

namespace Emx {

    class ReadMp4 {
    public:
        void Create(const char *path);

    private:
        Fmp4Reader m_mp4;
        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
    };

}

#endif //EMX_ReadMp4_HPP
