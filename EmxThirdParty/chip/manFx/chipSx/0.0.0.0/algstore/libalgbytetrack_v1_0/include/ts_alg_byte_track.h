/**
 * @file 	ts_alg_byte_track.h
 * @author 	yhc
 * @brief 	face track
 * @version 1.0
 * @date 	2023-10-13
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */

#ifndef _TS_ALG_BYTE_TRACK_H_
#define _TS_ALG_BYTE_TRACK_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"
#include "ts_alg_face_detect.h"

#define MAX_BYTE_TRACK_NUM     25		// max face detect number

typedef struct tsALG_TRACK_BOX_S{
	TS_U32	u32TrackId;
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32X;
    TS_FLOAT f32Y;
    TS_FLOAT f32Width;
    TS_FLOAT f32Height;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_TRACK_BOX_S;

typedef struct tsALG_BYTE_TRACK_S {
	TS_U32 u32TrackNum;
	ALG_TRACK_BOX_S stBox[MAX_BYTE_TRACK_NUM];
} ALG_BYTE_TRACK_S;

typedef struct tsALG_BYTE_TRACK_INPUT_S {
	TS_U32 u32MaxTrackNum;
	ALG_MODEL_INIT_S stFaceDet;     // face detect
	TS_U8 bRneOff;			        // not open RNE device, revent multiple operations
} ALG_BYTE_TRACK_INPUT_S;

typedef struct tsALG_BYTE_TRACK_RESULTS_S {
	TS_U32 u32Num;
	ALG_FACE_DETECT_RESULT_S stObjRes;
	ALG_BYTE_TRACK_S trackRes;
} ALG_BYTE_TRACK_RESULTS_S;

/**
 * @brief multi object track init
 *
 * @param ppHandle : Net handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_ByteTrack_Init(TS_VOID **ppHandle, ALG_BYTE_TRACK_INPUT_S *pParam);

/**
 * @brief multi object track process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input face address (w*h = 640*384)
 * @param pResult   : multi object track result
 *
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_ByteTrack_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_BYTE_TRACK_RESULTS_S *pResult);

/**
 * @brief multi object track exit
 *
 * @param pHandle   : Net handle
 *
 * @return error code : =0: ok   <0: error
 */
TS_S32 TS_ALG_ByteTrack_Exit(TS_VOID *pHandle);

/**
 * @brief set multi object track param
 *
 * @param pHandle : Net handle
 * @param pParam  : obj detect thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_ByteTrack_SetParam(TS_VOID *pHandle, ALG_FACE_DETECT_PARAM_S *pParam);

/**
 * @brief get multi object track param
 *
 * @param pHandle : Net handle
 * @param pParam  : obj detect thresh
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_ByteTrack_GetParam(TS_VOID *pHandle, ALG_FACE_DETECT_PARAM_S *pParam);

/**
 * @brief multi object track get version
 *
 * @return version string
 */
const TS_CHAR *TS_ALG_ByteTrack_GetVersion();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif		// _TS_ALG_BYTE_TRACK_H_

