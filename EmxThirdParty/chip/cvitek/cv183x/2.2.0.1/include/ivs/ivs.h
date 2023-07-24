#ifndef IVS_H
#define IVS_H

#include "cvi_comm_ivs.h"
#include "ive.h"
#include "mot_types.h"

typedef void *IVS_HANDLE;

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

CVI_S32 CVI_IVS_CreateHandle(IVS_HANDLE *handle, IVE_HANDLE ive_handle);
CVI_S32 CVI_IVS_DestroyHandle(IVS_HANDLE handle);

CVI_S32 CVI_IVS_MotionDetection_Init_Image(IVS_HANDLE handle,
                                           IVS_MD_MODEL_E md_model,
                                           IVS_MD_CONFIG_S md_config,
                                           IVE_IMAGE_S *init_image);

CVI_S32 CVI_IVS_MotionDetection_Init_Frame(IVS_HANDLE handle,
                                           IVS_MD_MODEL_E md_model,
                                           IVS_MD_CONFIG_S md_config,
                                           VIDEO_FRAME_INFO_S *init_frame);

CVI_S32
CVI_IVS_MotionDetection_Run_Image(IVS_HANDLE handle, IVS_MD_MODEL_E md_model,
                                  IVE_IMAGE_S *image, IVE_IMAGE_S *output_image,
                                  MD_BBOX_INFO_S *pstBbox, int MdMinArea);

CVI_S32 CVI_IVS_MotionDetection_Run_Frame(
    IVS_HANDLE handle, IVS_MD_MODEL_E md_model, VIDEO_FRAME_INFO_S *frame,
    IVE_IMAGE_S *output_image, MD_BBOX_INFO_S *pstBbox, int MdMinArea);

CVI_S32 CVI_IVS_MultiObjectTracking_Init(IVS_HANDLE handle);

CVI_S32 CVI_IVS_MultiObjectTracking_Run(IVS_HANDLE handle,
                                        MD_BBOX_INFO_S *cvi_ivs_bboxes,
                                        tracker_t *tracker);

CVI_S32 CVI_IVS_MultiObjectTracking_GetDefaultConfig(mot_config_t *mot_conf);

CVI_S32 CVI_IVS_MultiObjectTracking_SetConfig(const IVS_HANDLE handle,
                                              mot_config_t *mot_conf,
                                              bool show_config);

void CVI_IVS_FreeBBox(MD_BBOX_INFO_S *pstBbox);
void CVI_IVS_FreeTracker(tracker_t *tracker);

#ifdef __cplusplus
}
#endif

#endif
