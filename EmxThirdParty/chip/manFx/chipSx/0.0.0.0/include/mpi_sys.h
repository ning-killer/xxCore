/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_sys.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_SYS_H__
#define __MPI_SYS_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_sys.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


TS_S32 TS_MPI_SYS_Init(TS_VOID);
TS_S32 TS_MPI_SYS_Exit(TS_VOID);

TS_S32 TS_MPI_SYS_SetConfig(const MPP_SYS_CONFIG_S *pstSysConfig);
//TS_S32 TS_MPI_SYS_GetConfig(MPP_SYS_CONFIG_S *pstSysConfig);

TS_S32 TS_MPI_SYS_Bind(const MPP_CHN_S *pstSrcChn, const MPP_CHN_S *pstDestChn);
TS_S32 TS_MPI_SYS_UnBind(const MPP_CHN_S *pstSrcChn, const MPP_CHN_S *pstDestChn);
TS_S32 TS_MPI_SYS_GetBindbyDest(const MPP_CHN_S *pstDestChn, MPP_CHN_S *pstSrcChn);
TS_S32 TS_MPI_SYS_GetBindbySrc(const MPP_CHN_S *pstSrcChn, MPP_BIND_DEST_S *pstBindDest);

TS_S32 TS_MPI_SYS_GetVersion(MPP_VERSION_S *pstVersion);

TS_S32 TS_MPI_SYS_GetChipId(TS_U32 *pu32ChipId);

//TS_S32 TS_MPI_SYS_GetCustomCode(TS_U32 *pu32CustomCode);

/*
 * u64Base is the global PTS of the system.
 * ADVICE:
 * 1. Better to call TS_MPI_SYS_GetCurPTS on the host board to get u64Base.
 * 2. When os start up, call TS_MPI_SYS_InitPTSBase to set the init PTS.
 * 3. When media bussines is running, synchronize the PTS one time per minute
 *     by calling TS_MPI_SYS_SyncPTS.
 */
TS_S32 TS_MPI_SYS_GetCurPTS(TS_U64 *pu64CurPTS);
TS_S32 TS_MPI_SYS_InitPTSBase(TS_U64 u64PTSBase);
TS_S32 TS_MPI_SYS_SyncPTS(TS_U64 u64PTSBase);

/* alloc mmz memory in user context */
TS_S32 TS_MPI_SYS_MmzAlloc(TS_U64 *pu64PhyAddr, TS_VOID **ppVirAddr,
                           const TS_CHAR *strMmb, const TS_CHAR *strZone, TS_U32 u32Len);

/* alloc mmz memory with cache */
TS_S32 TS_MPI_SYS_MmzAlloc_Cached(TS_U64 *pu64PhyAddr, TS_VOID **ppVirAddr,
                                  const TS_CHAR *pstrMmb, const TS_CHAR *pstrZone, TS_U32 u32Len);

/* free mmz memory in user context */
TS_S32 TS_MPI_SYS_MmzFree(TS_U64 u64PhyAddr, TS_VOID *pVirAddr);

/* fulsh cache */
 TS_S32 TS_MPI_SYS_MmzFlushCache(TS_U64 u64PhyAddr, TS_VOID *pVirAddr, TS_U32 u32Size);

/*
 * Call the mmap function to map physical address to virtual address
 * The system function mmap is too complicated, so we packge it.
 */
TS_VOID *TS_MPI_SYS_Mmap(TS_U64 u64PhyAddr, TS_U32 u32Size);
TS_VOID *TS_MPI_SYS_MmapCache(TS_U64 u64PhyAddr, TS_U32 u32Size);
TS_S32 TS_MPI_SYS_Munmap(TS_VOID *pVirAddr, TS_U32 u32Size);
TS_S32 TS_MPI_SYS_MflushCache(TS_U64 u64PhyAddr, TS_VOID *pVirAddr, TS_U32 u32Size);

//TS_S32 TS_MPI_SYS_SetMemConfig(const MPP_CHN_S *pstMppChn, const TS_CHAR *pcMmzName);
//TS_S32 TS_MPI_SYS_GetMemConfig(const MPP_CHN_S *pstMppChn, TS_CHAR *pcMmzName);

/* Close all the FD which is used by sys module */
//TS_S32 TS_MPI_SYS_CloseFd(TS_VOID);

/* Get virtual meminfo according to virtual addr, should be in one process */
//TS_S32 TS_MPI_SYS_GetVirMemInfo(const void *pVirAddr, SYS_VIRMEM_INFO_S *pstMemInfo);

/* Set/get Scale coefficient level for VPSS/VGS */
//TS_S32 TS_MPI_SYS_SetScaleCoefLevel(const SCALE_RANGE_S *pstScaleRange,
//    const SCALE_COEFF_LEVEL_S *pstScaleCoeffLevel);
//TS_S32 TS_MPI_SYS_GetScaleCoefLevel(const SCALE_RANGE_S *pstScaleRange, SCALE_COEFF_LEVEL_S *pstScaleCoeffLevel);

/* Set/Get local timezone, range: [-86400, 86400] seconds (that is: [-24, 24] hours)  */
TS_S32 TS_MPI_SYS_SetTimeZone(TS_S32 s32TimeZone);
TS_S32 TS_MPI_SYS_GetTimeZone(TS_S32 *ps32TimeZone);

//TS_S32 TS_MPI_SYS_SetGPSInfo(const GPS_INFO_S *pstGPSInfo);
//TS_S32 TS_MPI_SYS_GetGPSInfo(GPS_INFO_S *pstGPSInfo);

//TS_S32 TS_MPI_SYS_SetTuningConnect(TS_S32 s32Connect);
//TS_S32 TS_MPI_SYS_GetTuningConnect(TS_S32 *ps32Connect);

TS_S32 TS_MPI_SYS_SetVIVPSSMode(const VI_VPSS_MODE_S *pstVIVPSSMode);
TS_S32 TS_MPI_SYS_GetVIVPSSMode(VI_VPSS_MODE_S *pstVIVPSSMode);

//TS_S32 TS_MPI_SYS_GetVPSSVENCWrapBufferLine(VPSS_VENC_WRAP_PARAM_S *pWrapParam, TS_U32 *pu32BufLine);

//TS_S32 TS_MPI_LOG_SetLevelConf(LOG_LEVEL_CONF_S *pstConf);
//TS_S32 TS_MPI_LOG_GetLevelConf(LOG_LEVEL_CONF_S *pstConf);

//TS_S32 TS_MPI_SYS_SetRawFrameCompressParam(const RAW_FRAME_COMPRESS_PARAM_S *pstCompressParam);
//TS_S32 TS_MPI_SYS_GetRawFrameCompressParam(RAW_FRAME_COMPRESS_PARAM_S *pstCompressParam);

//TS_S32 TS_MPI_LOG_SetWaitFlag(TS_BOOL bWait);

//TS_S32 TS_MPI_LOG_Read(TS_CHAR *pBuf, TS_U32 u32Size);

//TS_VOID TS_MPI_LOG_Close(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MPI_SYS_H__ */

