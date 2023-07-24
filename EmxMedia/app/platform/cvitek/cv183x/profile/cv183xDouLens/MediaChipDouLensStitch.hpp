//
// Created by cx on 2023/2/16.
//

#ifndef EMX_MEDIACHIPSTITCH_H
#define EMX_MEDIACHIPSTITCH_H

#include <core/EmxTypeDef.hpp>
#include <ive.h>
#include "cvi_comm_video.h"
#include "sample_comm.h"
#include "cvi_comm_gdc.h"


Emx::ErrCodeE InitStitch(int stitchMode, const Json::Value &json);

void DeInitStitch(void);

namespace Emx {

    typedef struct STIT_PARAM {
        CVI_U8 w_ir, w_y, ctrl;
        CVI_U8 mode;                // stitching mode: 1:cpu, 2:ive
        CVI_FLOAT homography[10];
        CVI_S32 corners[10];        // four points right
        CVI_S32 corners_left[10];   // four points left
        CVI_S32 roi_xywh[10];       // effective stitching region
        CVI_S32 stSize[4];          // left and right img size
        CVI_U8 stit_nbr;            // TBD
        CVI_S32 out_blend_start, out_blend_end; // blending start and end position in x-dir
        CVI_S32 blend_start_line, blend_end_line;
    } StitchParam;

    typedef struct _POINTXY {
        CVI_S32 x, y;
    } PointXY;


    class MediaChipDouLensStitch {

        MediaChipDouLensStitch();

        ~MediaChipDouLensStitch();

    public:
        static MediaChipDouLensStitch *GetInst();

        CVI_S32 StitchInit(StitchParam *param);

        CVI_S32 Stitch(VIDEO_FRAME_INFO_S *dstVideoFrameLft,
                       VIDEO_FRAME_INFO_S *dstVideoFrameRht,
                       VIDEO_FRAME_INFO_S *dstVideoFrameOut);

        CVI_S32 StitchSetRegion(int blending_start, int blending_end, StitchParam *param);

        CVI_S32 StitchFree();

    private:
        CVI_U8 m_mode;
        SIZE_S m_imgSize;
        CVI_S32 m_cornersLeft[10];
        CVI_S32 m_cornersRight[10];
        IVE_HANDLE m_handleIve;
        IVE_DST_IMAGE_S m_alpha; // alpha image
        IVE_IMAGE_S m_src1yuv;
        IVE_IMAGE_S m_src2yuv;
        IVE_IMAGE_S m_dstyuv;
    };
}

#endif //SV_VS9D6MM_MEDIACHIPSTITCH_H
