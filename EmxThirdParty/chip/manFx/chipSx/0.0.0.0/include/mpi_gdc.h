/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_gdc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */


#ifndef __MPI_GDC_H__
#define __MPI_GDC_H__

#include "ts_common.h"
#include "ts_comm_video.h"
#include "ts_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_GDC_BeginJob(GDC_HANDLE *phHandle);
TS_S32 TS_MPI_GDC_SetConfig(GDC_HANDLE hHandle,const FISHEYE_JOB_CONFIG_S *pstJobConfig);
TS_S32 TS_MPI_GDC_AddCorrectionTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *p_sttask, const FISHEYE_ATTR_S * pstFisheyeAttr);
TS_S32 TS_MPI_GDC_AddPMFTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S * p_sttask, const GDC_PMF_ATTR_S * pstGdcPmfAttr);
TS_S32 TS_MPI_GDC_EndJob(GDC_HANDLE hHandle);
TS_S32 TS_MPI_GDC_CancelJob(GDC_HANDLE hHandle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* end of __MPI_GDC_H__ */



