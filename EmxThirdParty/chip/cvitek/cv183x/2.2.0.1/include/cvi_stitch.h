#ifndef CVI_STITCH_H
#define CVI_STITCH_H

#include "cvi_comm_video.h"
#include "sample_comm.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _STIT_PARAM {
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
} STIT_PARAM;


/**
 * @brief This function for initialization
 * @param[in] model_folder: one folder path for training data
 * @param[in] param: model info
 * @return
 */

CVI_S32 CVI_STITCH_Init(CVI_CHAR *model_folder, STIT_PARAM *param);


/**
 * @brief This function for stitching two images
 * @param[in/out] dstVideoFrameLft: left YUV420 warped image
 * @param[in/out] dstVideoFrameRht: right YUV420 warped image
 * @param[out] dstVideoFrameOut: final YUV420 blending image
 * @param[in] param: mode settings
 * @param[in] param: mode 2, hw ive blending, Lft: in/stitch_w, Rht: in/stitch_w, Out: out/stitch_w (stitch_w<wx2);
 *                   the sizes of Lft, Rht, and Out are same
 * @return <0: fail
 */


CVI_S32 CVI_STITCH( VIDEO_FRAME_INFO_S *dstVideoFrameLft,
                    VIDEO_FRAME_INFO_S *dstVideoFrameRht,
				    VIDEO_FRAME_INFO_S *dstVideoFrameOut);


/**
 * @brief This function for changing blending area
 * @param[in] blending_start, the starting position you want to use
 * @param[in] blending_end, the end position you want to use
 * @param[in] param: model info
 * @return
 */
CVI_S32 CVI_STITCH_set_region( int blending_start, int blending_end, STIT_PARAM *param);


/**
 *@brief This function for free all
 */
CVI_S32 CVI_STITCH_Free();


#ifdef __cplusplus
}
#endif

#endif
