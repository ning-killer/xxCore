/**
 * @file ts_alg_face_solution.h
 * @author niuyf
 * @brief face solution
 * @version 1.0
 * @date 2022-10-16
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */
#ifndef _TS_ALG_FACE_SOLUTION_H_
#define _TS_ALG_FACE_SOLUTION_H_

#include "ts_alg_type.h"

#define MAX_FACE_SOLUTION_NUM     25		// max face detect number

typedef struct tsALG_FACE_DETECT_BOX_S{
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_FACE_DETECT_BOX_S;

typedef struct tsALG_FACE_DETECT_S{
	TS_U32 u32FaceNum;
	ALG_FACE_DETECT_BOX_S stBox[MAX_FACE_SOLUTION_NUM];
} ALG_FACE_DETECT_S;

typedef struct tsALG_FACE_OCCLUSION_S{
	TS_U8 bOccusion;  //1遮挡，0未遮挡
	TS_FLOAT f32Score; //得分
} ALG_FACE_OCCLUSION_S;

typedef struct tsALG_FACE_BIGANGLE_S{
	TS_U8 bBigAngle; //1大角度，0未大角度
	TS_FLOAT f32Score; //得分
} ALG_FACE_BIGANGLE_S;

typedef struct tsALG_FACE_HEADPOSE_S {
	TS_U8 bGoodpose; //1未抬头低头
	TS_FLOAT f32Headpose[3]; //得分
} ALG_FACE_HEADPOSE_S;

typedef struct tsALG_FACE_MASKCLASSIFY_S {
	TS_U8 bClass; //1有口罩眼睛，0没有
	TS_FLOAT f32ClassScore; //得分
} ALG_FACE_MASKCLASSIFY_S;

typedef struct tsALG_FACE_SOLUTION_RESULT_S{
	TS_U32 u32Num;
	ALG_FACE_DETECT_S stDetect;										//人脸检测结果
	ALG_FACE_OCCLUSION_S stOcclusion[MAX_FACE_SOLUTION_NUM];		//人脸遮挡分析结果
	ALG_FACE_BIGANGLE_S  stBigAngle[MAX_FACE_SOLUTION_NUM];		//人脸遮挡分析结果
	ALG_FACE_HEADPOSE_S  stHeadPose[MAX_FACE_SOLUTION_NUM];			//人脸头部姿态分析结果
	ALG_FACE_MASKCLASSIFY_S stMaskClassify[MAX_FACE_SOLUTION_NUM];	//人脸口罩眼镜检测结果
} ALG_FACE_SOLUTION_RESULT_S;

typedef struct tsALG_FACE_SOLUTION_PARAM_S{
	TS_FLOAT f32FaceDetThr;
	TS_FLOAT f32OccusionThr;
	TS_FLOAT f32BigAngleThr;
	TS_FLOAT f32PoseThr[3];
	TS_FLOAT f32ClassifyThr;
} ALG_FACE_SOLUTION_PARAM_S;

typedef struct tsALG_FACE_SOLUTION_INIT_S{
	TS_U32 u32ImgW;
	TS_U32 u32ImgH;

	TS_U8 bOccusion;
	TS_U8 bBigAngle;
	TS_U8 bHeadpose;
	TS_U8 bMaskclassify;

	ALG_MODEL_INIT_S stFaceDet;			//人脸检测
	ALG_MODEL_INIT_S stFaceOcclusion;	//人脸遮挡分析
	ALG_MODEL_INIT_S stBigAngle;		//人脸大角度预判
	ALG_MODEL_INIT_S stHeadPose;		//人脸姿态分析
	ALG_MODEL_INIT_S stMaskclassify;	//人脸口罩分类
	TS_U8 bRneOff;					  	// not open RNE device, revent multiple operations
} ALG_FACE_SOLUTION_INIT_S;


/**
 * @brief face solution init
 *
 * @param ppHandle : solution handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_FaceSolution_Init(TS_VOID **ppHandle, ALG_FACE_SOLUTION_INIT_S *pParam);

/**
 * @brief face solustion process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input src image
 * @param pImageDet : input detect image
 * @param pResult   : Face solustion result
 *
 * @return  0 or error code
 */
TS_S32 TS_ALG_FaceSolution_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_FACE_SOLUTION_RESULT_S *pResult);

/**
 * @brief face solustion exit
 *
 * @param pHandle   : Net handle
 *
 * @return  0 or error code
 */
TS_S32 TS_ALG_FaceSolution_Exit(TS_VOID *pHandle);

/**
 * @brief set face solustion param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_FaceSolution_SetParam(TS_VOID *pHandle, ALG_FACE_SOLUTION_PARAM_S *pParam);

/**
 * @brief get face solustion param
 *
 * @param handle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_FaceSolution_GetParam(TS_VOID *pHandle, ALG_FACE_SOLUTION_PARAM_S *pParam);

/**
 * @brief face solustion get version
 *
 * @return version string
 */
const TS_CHAR* TS_ALG_FaceSolution_GetVersion(TS_VOID);

#endif
