#ifndef __TS_RNE_PLAT_PORT_H__
#define __TS_RNE_PLAT_PORT_H__

#include "ts_rne_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief RNE_PORT_ADDR_S struct
 */
typedef struct tsRNE_PORT_ADDR_S {
    TS_SIZE_T szBaseAddr;
    TS_SIZE_T szNNpu;
    TS_SIZE_T szMemPhyAddr;  // rne dma ddr physical addr
    TS_SIZE_T szMemVirAddr;  // rne dma ddr virtual addr
    TS_U32 u32MemSize;       // rne dma ddr mem size
} RNE_PORT_ADDR_S;

/**
 * @brief RNE_DRIVER_S struct
 */
typedef struct tsRNE_DRIVER_S {
    TS_S32 (*fnpRneInit)(RNE_PORT_ADDR_S *port);
    TS_VOID (*fnpRneFree)(TS_VOID);
    TS_VOID (*fnpRneProcOpt)(TS_VOID *optional);
    TS_U32 (*fnpRneIsForwardDone)(TS_S32 npuIdx, TS_S32 timeOutMs);
    TS_VOID (*fnpRneClearForwardDone)(TS_S32 npuIdx);
} RNE_DRIVER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_PLAT_PORT_H__ */
