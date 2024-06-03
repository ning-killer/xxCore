/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_ae.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "ts_comm_isp.h"
#include "ts_comm_3a.h"
#include "ts_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/* The interface of ae lib register to isp. */
TS_S32 TS_MPI_AE_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);
TS_S32 TS_MPI_AE_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
TS_S32 TS_MPI_AE_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                   AE_SENSOR_REGISTER_S *pstRegister);
TS_S32 TS_MPI_AE_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

TS_S32 TS_MPI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
TS_S32 TS_MPI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr);

TS_S32 TS_MPI_ISP_SetWDRExposureAttr(VI_PIPE ViPipe, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);
TS_S32 TS_MPI_ISP_GetWDRExposureAttr(VI_PIPE ViPipe, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);

TS_S32 TS_MPI_ISP_SetHDRExposureAttr(VI_PIPE ViPipe, const ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);
TS_S32 TS_MPI_ISP_GetHDRExposureAttr(VI_PIPE ViPipe, ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr);

TS_S32 TS_MPI_ISP_SetAERouteAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstAERouteAttr);
TS_S32 TS_MPI_ISP_GetAERouteAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteAttr);

TS_S32 TS_MPI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo);

TS_S32 TS_MPI_ISP_SetIrisAttr(VI_PIPE ViPipe, const ISP_IRIS_ATTR_S *pstIrisAttr);
TS_S32 TS_MPI_ISP_GetIrisAttr(VI_PIPE ViPipe, ISP_IRIS_ATTR_S *pstIrisAttr);

TS_S32 TS_MPI_ISP_SetDcirisAttr(VI_PIPE ViPipe, const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
TS_S32 TS_MPI_ISP_GetDcirisAttr(VI_PIPE ViPipe, ISP_DCIRIS_ATTR_S *pstDcirisAttr);

TS_S32 TS_MPI_ISP_SetPirisAttr(VI_PIPE ViPipe, const ISP_PIRIS_ATTR_S *pstPirisAttr);
TS_S32 TS_MPI_ISP_GetPirisAttr(VI_PIPE ViPipe, ISP_PIRIS_ATTR_S *pstPirisAttr);

TS_S32 TS_MPI_ISP_SetAERouteAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
TS_S32 TS_MPI_ISP_GetAERouteAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
