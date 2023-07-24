/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_gdc.h
 * Description:
 *   gdc interfaces.
 */

#ifndef __CVI_GDC_H__
#define __CVI_GDC_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Begin a gdc job,then add task into the job,gdc will finish all the task in the job.
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_BeginJob(GDC_HANDLE *phHandle);

/* End a job,all tasks in the job will be submmitted to gdc
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_EndJob(GDC_HANDLE hHandle);

/* Cancel a job ,then all tasks in the job will not be submmitted to gdc
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_CancelJob(GDC_HANDLE hHandle);

/* Add a fisheye task to a gdc job
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @param pstTask: to describe what to do
 * @param pstFisheyeAttr: for further settings
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_AddCorrectionTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
				  const FISHEYE_ATTR_S *pstFisheyeAttr);

/* Add a rotation task to a gdc job
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @param pstTask: to describe what to do
 * @param enRotation: for further settings
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_AddRotationTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, ROTATION_E enRotation);

CVI_S32 CVI_GDC_AddAffineTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const AFFINE_ATTR_S *pstAffineAttr);

CVI_S32 CVI_GDC_AddLDCTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
	const LDC_ATTR_S *pstLDCAttr, ROTATION_E enRotation);

// color night vision
CVI_S32 CVI_GDC_AddCnvWarpTask(const float *pfmesh_data, GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
			       const FISHEYE_ATTR_S *pstAffineAttr, bool *bReNew);

CVI_S32 CVI_GDC_AddCorrectionTaskCNV(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
		const FISHEYE_ATTR_S *pstFishEyeAttr, uint8_t *p_tbl, uint8_t *p_idl, uint32_t *tbl_param);

/* set meshsize for rotation only
 *
 * @param nMeshHor: mesh counts horizontal
 * @param nMeshVer: mesh counts vertical
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_SetMeshSize(int nMeshHor, int nMeshVer);

CVI_S32 CVI_GDC_DumpMesh(MESH_DUMP_ATTR_S *pMeshDumpAttr);
CVI_S32 CVI_GDC_LoadMesh(MESH_DUMP_ATTR_S *pMeshDumpAttr, char *grid_name, CVI_BOOL HomoEnable);

CVI_S32 CVI_GDC_SetGridInfo(char *grid_name, int *info, int *pgrid_src, int *pgrid_dst
	, int *pmesh_src, int *pmesh_dst, int *pnode_src, int *pnode_dst);

/* gen meshdata for homograph
 *
 * @param imgSize: image size
 * @param meshSize: mesh size for every grid
 * @param meshSize: 3x3 homo matrix
 * @param bindName: define a bindName by user for this meshdata
 * @param savePath: The save path when the user needs to save the grid information,
 * NULL means do not save any grid information
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_GenMeshDataAll(SIZE_S imgSize, SIZE_S meshSize, float *matrix
	, char *bindName, char *savePath);

/* gen meshdata for region by matrix */
CVI_S32 CVI_GDC_GenRegionMeshDataByMatrix(SIZE_S imgSize, SIZE_S meshSize
	, char *regionName, int regionX, int regionY, int regionW, int regionH
	, float *matrix, char *savePath);

/* gen meshdata for region by LUT bin */
CVI_S32 CVI_GDC_GenRegionMeshDataByBin(SIZE_S imgSize, SIZE_S meshSize
	, char *regionName, int regionX, int regionY, int regionW, int regionH
	, char *pathMapX, char *pathMapY, char *savePath);

/* gen meshdata for stitching */
CVI_S32 CVI_GDC_GenStitchMeshData(SIZE_S imgSize, SIZE_S meshSize, float *matrix
	, char *bindName, char *savePath);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_GDC_H__ */
