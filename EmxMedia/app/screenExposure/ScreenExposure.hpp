//
// Created by cx on 2023/6/29.
//

#ifndef EMX_SCREENEXPOSURE_HPP
#define EMX_SCREENEXPOSURE_HPP

#include "cvi_comm_video.h"

namespace Emx {

#define AE_CHANNEL_GB              0
#define AE_CHANNEL_B               1
#define AE_CHANNEL_GR              2
#define AE_CHANNEL_R               3
#define AE_GRID_ROW                15
#define AE_GRID_COLUMN             17
#define FACE_AE_MIN_CNT            6            //GetAEStatistics Min Size

#define FACE_TARGET_EVBIAS_L_RANGE 512          //min expAttr avoid too dark
#define FACE_TARGET_EVBIAS_H_RANGE (1024 * 4)   //max expAttr avoid too bright

    typedef struct {
        unsigned int x1;
        unsigned int y1;
        unsigned int x2;
        unsigned int y2;
    } positionMsg;

    class ScreenExposure {
        ScreenExposure();

        ~ScreenExposure();

    public:
        static ScreenExposure *GetInst();

        int
        StartScreenExposure(VIDEO_FRAME_INFO_S *pstFrame, RECT_S *pstRegion, int regionNum, int targetLuma, VI_PIPE chn);

        void EndScreenExposure(VI_PIPE chn);

    private:
        int GetRegionAE(positionMsg pos, CVI_U32 Width, CVI_U32 Height);

        int GetRegionLuma(positionMsg pos, VIDEO_FRAME_INFO_S *pstFrame);

    private:
        unsigned short m_defaultEVBias;


    };
}
#endif //EMX_SCREENEXPOSURE_HPP
