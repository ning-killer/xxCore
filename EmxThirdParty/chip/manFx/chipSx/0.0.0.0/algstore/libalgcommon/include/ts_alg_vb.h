/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_VB_H_
#define _TS_ALG_VB_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if 0

TS_S32 TS_ALG_SYS_Init(TS_U32 blkSize, TS_U32 blkCnt);

TS_S32 TS_ALG_SYS_Exit(void);

TS_S32 TS_ALG_MmzAlloc(TS_U64 *pu64PhyAddr, TS_VOID **ppVirAddr,
                                  const TS_CHAR *pstrMmb, const TS_CHAR *pstrZone, TS_U32 u32Len);

TS_S32 TS_ALG_MmzFree(TS_U64 u64PhyAddr, TS_VOID* pVirAddr);

#else

typedef int32_t vb_pool;

TS_S32 TS_ALG_VB_Init(TS_U32 blkSize, TS_U32 blkCnt);


TS_S32 TS_ALG_VB_Exit(void);

TS_S32 TS_ALG_VB_CreatePool(vb_pool *poolId, TS_U32 blkSize, TS_U32 blkCnt);

TS_S32 TS_ALG_VB_ReleasePool(vb_pool poolId);
#if (defined(TX521X) || defined(TX5112X) || defined(TX5215) || defined(TX5239))
TS_S32 TS_ALG_VB_GetBlock(vb_pool poolId, TS_U32 blkSize, TS_U32 *phyAddr, TS_VOID **virAddr);
#endif

#if (defined(TX5368X))
TS_S32 TS_ALG_VB_GetBlock(vb_pool poolId, TS_U32 blkSize, TS_U64 *phyAddr, TS_VOID **virAddr);
#endif

TS_S32 TS_ALG_VB_ReleaseBlock(TS_U64 phyAddr);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_VB_H_ */
