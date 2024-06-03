#ifndef __TS_RNE_DEVICE_H__
#define __TS_RNE_DEVICE_H__

#include <stddef.h>
#include "ts_rne_plat_port.h"
#include "ts_rne_type.h"

BEGIN_DECLS

/**
 * @brief the interface of register all GP layers
 *
 * @param none
 *
 * @return none
 */
typedef TS_S32 (*TS_MPI_TRP_RNE_RegisterAllGpLayers)(TS_VOID);

/**
 * @brief RNE_BLOB_FORMAT_E enum
 */
typedef enum tsRNE_BLOB_FORMAT_E {
    RNE_BLOB_N_H_W_Cstride, // Cstride=rne_c_stride()
    RNE_BLOB_N_Cn_N_H_W_Cx, // Cn=Cstride/Cx, Cx=8or32
    RNE_BLOB_N_H_W_C,
} RNE_BLOB_FORMAT_E;

/**
 * @brief RNE_BLOB_S struct, all blob data shape order is [h:w:c_stride], inlude result
 */
typedef struct tsRNE_BLOB_S {
    TS_S32 s32N;               /**< number */
    TS_S32 s32H;               /**< height */
    TS_S32 s32W;               /**< width */
    TS_S32 s32C;               /**< channel */
    TS_CHAR *cpName;             /**< blob name */
    TS_VOID *vpAddr;           /**< data address */
    TS_S32 s32Offs;               /**< data offset */
    TS_S32 s32BitNum;          /**< bit number */
    TS_RNE_BOOL bIsUnsigned;       /**< is or not unsigned */
    TS_RNE_BOOL bIsJoined;         /**< is or not joined */
    TS_S32 *s32pQ;             /**< -1 is a floating point number and other integers */
    TS_FLOAT *fCoeff;          /**< data coefficient */
    RNE_BLOB_FORMAT_E eFormat; /**< data format */
} RNE_BLOB_S;

/**
 * @brief RNE_BLOBS_S struct
 */
typedef struct tsRNE_BLOBS_S {
    TS_U32 u32NBlob;     /**< RNE_BLOB_S array number */
    RNE_BLOB_S *stpBlob; /**< RNE_BLOB_S array pointer */
} RNE_BLOBS_S;

/**
 * @brief RNE_BIN_DATA_S struct
 */
typedef struct tsRNE_BIN_DATA_S {
    TS_SIZE_T szLen; /**< Number of bytes in the `data` field. */
    TS_U8 *u8pData;  /**< Data bytes. */
} RNE_BIN_DATA_S;

/**
 * @brief GP forward interface
 *
 * @param input input blobs of the GP layer
 * @param output output blobs of the GP layer
 * @param extraData extern data of the GP layer
 * @param userData user data registered by TS_MPI_TRP_RNE_RegisterGpUserData
 *
 * @return none
 */
typedef TS_VOID (*TS_MPI_TRP_RNE_GpLayerHandler)(RNE_BLOBS_S *input, RNE_BLOBS_S *output, RNE_BIN_DATA_S *extraData,
                                                 TS_VOID *userData);

/**
 * @brief rne GP layers handler
 */
typedef struct tsRNE_GP_LAYERS_HD_S {
    TS_MPI_TRP_RNE_GpLayerHandler *fnpGpLayerHd; /**< GP layers handler arrary */
    TS_S32 s32GpLayerTypeNum;                    /**< Total number of GP layers handler arrary */
    TS_S32 s32UserLayerTypeNum;                  /**< User registered number of GP layers handler arrary */
} RNE_GP_LAYERS_HD_S;

/**
 * @brief get all GP layers handler
 *
 * @return GP layers handler
 */
RNE_GP_LAYERS_HD_S *TS_MPI_TRP_RNE_GetGpLayersHd(TS_VOID);

/**
 * @brief initialize GP layers number to register
 *
 * @param num the number to regitser GP layers
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_InitGpLayerNum(TS_S32 num);

/**
 * @brief register the forward function of GP layer dependent on layer_type
 *
 * @param layerType layer type
 * @param gpLayerCallback GP layer forward call back
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_RegisterGpLayer(TS_S32 layerType, TS_MPI_TRP_RNE_GpLayerHandler gpLayerCallback);

/**
 * @brief open all rne(NPU) devices
 *
 * @param driver rne drivers, if use default NULL, then link default,
 * @param registerGpLayers the init function(callback) of register all GP layers, default NULL,
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_OpenDevice(RNE_DRIVER_S *driver, TS_MPI_TRP_RNE_RegisterAllGpLayers registerGpLayers);

/**
 * @brief close all rne(NPU) devices,this function is called only once
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_CloseDevice(TS_VOID);

/**
 * @brief all rne(NPU) devices is opened
 *
 * @return true: is opened false: is closed
 */
TS_RNE_BOOL TS_MPI_TRP_RNE_DeviceIsOpened(TS_VOID);

/**
 * @brief  get core number of rne device
 *
 * @return  rne core number
 * -1 : error, need open device firstly
 */
TS_S32 TS_MPI_TRP_RNE_GetCoreNum(TS_VOID);

/**
 * @brief  alloc continuous physical memory for linux(with mmu) platform
 * only for rne net self_buf and params(weight)
 *
 * @param len  length(Byte)
 *
 * @return  pointer virtual address for user, NULL: failed
 */
TS_VOID *TS_MPI_TRP_RNE_AllocLinearMem(TS_SIZE_T len);

/**
 * @brief  free continuous physical memory for linux(with mmu) platform
 * only for rne net self_buf and params(weight)
 *
 * @param ptr  pointer from rne_linear_mem_alloc
 */
TS_VOID TS_MPI_TRP_RNE_FreeLinearMem(TS_VOID *ptr);

/**
 * @brief convert virtual address to physical address
 *
 * @param addr_vir  virtual address
 *
 * @return  physical address
 */
size_t TS_MPI_TRP_RNE_VirtualToPhysicalAddress(size_t addr_vir);

/**
 * @brief convert physical address to virtual address
 *
 * @param addr_phy  physical address
 *
 * @return  virtual address
 */
size_t TS_MPI_TRP_RNE_PhysicalToVirtualAddress(size_t addr_phy);

END_DECLS

#endif /* __TS_RNE_DEVICE_H__ */
