/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_awb.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "ts_comm_isp.h"
#include "ts_comm_3a.h"
#include "ts_awb_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* The interface of awb lib register to isp. */
TS_S32 TS_MPI_AWB_Register(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);
TS_S32 TS_MPI_AWB_UnRegister(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
TS_S32 TS_MPI_AWB_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                    AWB_SENSOR_REGISTER_S *pstRegister);
TS_S32 TS_MPI_AWB_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

TS_S32 TS_MPI_ISP_SetWBAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr);
TS_S32 TS_MPI_ISP_GetWBAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr);

TS_S32 TS_MPI_ISP_SetAWBAttrEx(VI_PIPE ViPipe, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);
TS_S32 TS_MPI_ISP_GetAWBAttrEx(VI_PIPE ViPipe, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);

TS_S32 TS_MPI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);
TS_S32 TS_MPI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

TS_S32 TS_MPI_ISP_SetSaturationAttr(VI_PIPE ViPipe, const ISP_SATURATION_ATTR_S *pstSatAttr);
TS_S32 TS_MPI_ISP_GetSaturationAttr(VI_PIPE ViPipe, ISP_SATURATION_ATTR_S *pstSatAttr);

TS_S32 TS_MPI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);
TS_S32 TS_MPI_ISP_GetAwbInfo(VI_PIPE ViPipe, ISP_AWB_CONFIG_S *pstAwbInfo);

TS_S32 TS_MPI_ISP_CalGainByTemp(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr, TS_U16 u16ColorTemp, TS_S16 s16Shift, TS_U16 *pu16AWBGain);
TS_S32 TS_MPI_ISP_SetSpecAwbAttr(VI_PIPE ViPipe, const ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);
TS_S32 TS_MPI_ISP_SetCaaControlAttr(VI_PIPE ViPipe, const ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
TS_S32 TS_MPI_ISP_GetCaaControlAttr(VI_PIPE ViPipe, ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
TS_S32 TS_MPI_ISP_GetSpecAwbAttr(VI_PIPE ViPipe, ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
