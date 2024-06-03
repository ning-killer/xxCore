#ifndef __TS_RNE_C_API_H__
#define __TS_RNE_C_API_H__

#include "ts_rne_device.h"
#include "ts_rne_plat_port.h"
#include "ts_rne_time.h"
#include "ts_rne_type.h"
#include <stddef.h>

BEGIN_DECLS

typedef enum tsRNE_NET_INPUT_TYPE_E {
    RNE_NET_INPUT_TYPE_FLOAT32_HWC = 0,
    RNE_NET_INPUT_TYPE_FLOAT_HWC = 0,
    RNE_NET_INPUT_TYPE_FLOAT32_CHW = 1,
    RNE_NET_INPUT_TYPE_FLOAT_CHW = 1,
    RNE_NET_INPUT_TYPE_INT8_HWC = 2,
    RNE_NET_INPUT_TYPE_INT_HWC = 2,
    RNE_NET_INPUT_TYPE_INT8_CHW = 3,
    RNE_NET_INPUT_TYPE_INT_CHW = 3,
    RNE_NET_INPUT_TYPE_FORMAT_0RGB = 4,
    RNE_NET_INPUT_TYPE_FORMAT_GREY = 5,
    RNE_NET_INPUT_TYPE_INT_HWCSTRIDE = 6
} RNE_NET_INPUT_TYPE_E;

typedef enum tsRNE_NET_OUTPUT_TYPE_E {
    RNE_NET_OUTPUT_TYPE_FORMAT_NONE,
    RNE_NET_OUTPUT_TYPE_FORMAT_NHWC_TO_NCHW
} RNE_NET_OUTPUT_TYPE_E;

/**
 * @brief RNE_NET_S struct
 * user must set the uncared member to 0 before net init
 */
typedef struct tsRNE_NET_S {
    /**
     * @brief input data type
     */
    RNE_NET_INPUT_TYPE_E eInputType;
    /**
     * @brief output data type
     */
    RNE_NET_OUTPUT_TYPE_E eOutputType;
    /**
     * @brief net parameters, inlcude weight and quantization
     */
    TS_U8 *u8pParams;
    /**
     * @brief net separate weight, NULL: params include weight
     */
    TS_U8 *u8pWeight;
    /**
     * @brief net config len(bytes)
     */
    TS_S32 s32GraphLen;
    /**
     * @brief net config
     */
    TS_U8 *u8pGraph;
    /**
     * @brief debug layer name, if you want debug some layer ouput, set debug_layer_name, or set it to NULL
     */
    TS_CHAR *cpDebugLayerName;
    /**
     * @brief gp layer user_data list, you just only need initialize it to NULL
     */
    TS_VOID *vpGpUserData;
    /**
     * @brief inner net description, you just only need initialize it to NULL
     */
    TS_VOID *vpInnerNd;
    /**
     * @brief user self buffer, NULL: use common buffer,
     * else use self_buf alloced by user before rne_net_init_config
     */
    TS_VOID *vpSelfBuf;
    /**
     * @brief net run number of task(core)
     * range: 0~MAX_TASKS, 0 or 1: indicate 1 task
     * setting it is valid before net init
     */
    TS_S32 s32NTask;
} RNE_NET_S;

typedef struct tsRNE_GP_LAYER_S {
    TS_S32 s32LayerId;
    TS_CHAR *cpLayerName;
    TS_S32 s32LayerType;
    TS_CHAR *cpTypeName;
} RNE_GP_LAYER_S;

typedef struct tsRNE_GP_LAYERS_S {
    TS_U32 u32LayersNum;
    RNE_GP_LAYER_S *stpLayerInfo;
} RNE_GP_LAYERS_S;

typedef enum tsRNE_ASYNC_EXE_STATE {
    RNE_NN_FORWARD_INIT,
    RNE_NN_FORWARD_FEED,
    RNE_NN_FORWARD_READY,
    RNE_NN_FORWARD_RUNNING,
    RNE_NN_FORWARD_DONE,
    RNE_NN_FORWARD_SUSPEND,
    RNE_NN_FORWARD_RESUME,
    RNE_NN_FORWARD_WAIT,
    RNE_NN_FORWARD_IDLE,
    RNE_NN_FORWARD_EXCEPTION
} RNE_ASYNC_EXE_STATE;

/** async executor state transfer process
 *                        RESUME <- WAIT <- SUSPEND
 *           priority higher |                ^
 *                           v                | priority lower
 * normal: INIT -> FEED -> READY     ->     RNNING -> DONE -> IDLE
 */
typedef struct tsRNE_ASYNC_EXECUTOR {
    RNE_ASYNC_EXE_STATE state;
    ts_s32 n_task_done;
    ts_s32 *core_idx;
    RNE_NET_S *nn;
    ts_u8 *nn_in;
    ts_u8 *nn_out;
    ts_s32 priority; /** range:[0, 255], higher->lower:0>1>2>...>255 */
    ts_void *context;
} RNE_ASYNC_EXECUTOR;

/**
 * @brief get the list of network software layers
 *
 * @param net net pointer
 *
 * @return net software layers
 */
RNE_GP_LAYERS_S *TS_MPI_TRP_RNE_GetGpLayers(RNE_NET_S *net);

/**
 * @brief initialize NN net
 * note before call it, the rne_device_open must be called
 * after call it, optional call rne_net_once_load_params,this function is called only once
 *
 * @param net NN net pointer
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_LoadModel(RNE_NET_S *net);

/**
 * @brief release NN net
 * note after call it, the rne_device_close can be called
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_UnloadModel(RNE_NET_S *net);

/**
 * @brief  once load params of a NN net
 * call it after rne_net_init_config
 * it do nothing if the net has't once load attribute
 *
 * @param net  NN net pointer
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_OnceLoad(RNE_NET_S *net);

/**
 * @brief net forward
 *
 * @param net net pointer
 *
 * @return if debug_layer_name of net is not NULL and correct, return the output blobs of it,
 * else return result blobs of net.
 * if the return is NULL, there is an anomaly.
 */
RNE_BLOBS_S *TS_MPI_TRP_RNE_Forward(RNE_NET_S *net);

/**
 * @brief acquire net blob data buffer size,
 *
 * @param net net pointer
 *
 * @return >0: successful  0: fails
 */
TS_U32 TS_MPI_TRP_RNE_GetBlobsBufSize(RNE_NET_S *net);

/**
 * @brief acquire result blobs of net or debug layer, call it after net init
 * @if debug_layer_name of net is not NULL and correct, return the output blobs of the debug layer,
 * else return result blobs of the net.
 *
 * @param net net pointer
 *
 * @return net result blobs
 */
RNE_BLOBS_S *TS_MPI_TRP_RNE_GetResultBlobs(RNE_NET_S *net);

/**
 * @brief acquire input blobs of net, call it after net init
 *
 * @param net net pointer
 *
 * @return net input blobs
 */
RNE_BLOBS_S *TS_MPI_TRP_RNE_GetInputBlobs(RNE_NET_S *net);

/**
 * @brief acquire hidden blobs of net, call it after net init
 *
 * @param net net pointer
 *
 * @return net hidden blobs
 */
RNE_BLOBS_S *TS_MPI_TRP_RNE_GetHideBlobs(RNE_NET_S *net);

/**
 * @brief register the user data of GP layer dependent on layer id,
 * call it before net init
 *
 * @param net net pointer
 * @param layerId layer ID
 * @param userData user data pointer
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_RegisterGpUserData(RNE_NET_S *net, TS_S32 layerId, TS_VOID *userData);

/**
 * @brief caculate the stored aligned channel by the channel of blob
 *
 * @param chan valid channel data
 * @param bitNum data bit number
 * @param isJoined data is or not joined
 *
 * @return the stored aligned channel
 */
TS_S32 TS_MPI_TRP_RNE_CStride(TS_S32 chan, TS_S32 bitNum, TS_RNE_BOOL isJoined);

/**
 * @brief get the network resource version
 *
 * @param net net pointer
 *
 * @return the network resource version
 */
const TS_CHAR *TS_MPI_TRP_RNE_GetResVersion(RNE_NET_S *net);

/**
 * @brief get quantization weight file size
 *
 * @param net net pointer
 *
 * @return the quantization weight file size
 */
TS_S64 TS_MPI_TRP_RNE_GetWeightSize(RNE_NET_S *net);

/**
 * @brief get weight file size
 *
 * @param net net pointer
 *
 * @return the weight file size
 */
TS_S64 TS_MPI_TRP_RNE_GetWeightWSize(RNE_NET_S *net);

/**
 * @brief get quantization file size
 *
 * @param net net pointer
 *
 * @return the quantization file size
 */
TS_S64 TS_MPI_TRP_RNE_GetWeightMSize(RNE_NET_S *net);

/**
 * @brief get quantization instruction file size
 *
 * @param net net pointer
 *
 * @return the quantization instruction file size
 */
TS_S64 TS_MPI_TRP_RNE_GetCfgSize(RNE_NET_S *net);

/**
 * @brief : network time-consuming binding
 *
 * @param net net pointer
 * @param time_state obj
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_NetBindTimeState(RNE_NET_S *net, RNE_TIME_STATES_S *time_state);

/**
 * @brief  set the net input blobs addr,
 * all input blobs memory should be continuous in order
 * call it after rne_net_init_config
 *
 * @param net  NN pointer
 * @param addr  the first input blob addr
 * @param is_phy_addr  is or not physical address
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_SetInputBlobsAddr(RNE_NET_S *net, TS_VOID *addr, TS_RNE_BOOL is_phy_addr);

/**
 * @brief  set the net weight addr,
 * all weight memory should be continuous in order
 * call it after rne_net_init_config
 *
 * @param net  NN pointer
 * @param addr  the weight addr
 * @param is_phy_addr  is or not physical address
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_SetWeightAddr(RNE_NET_S *net, TS_VOID *addr, TS_RNE_BOOL is_phy_addr);

/**
 * @brief  get the blob size,
 *
 * @param blobs  the blob
 *
 * @return blob size
 */
TS_SIZE_T TS_MPI_TRP_RNE_GetBlobSize(RNE_BLOB_S *blob);

/**
 * @brief  get the blobs size,
 *
 * @param blobs  the blobs
 *
 * @return blobs size
 */
TS_SIZE_T TS_MPI_TRP_RNE_GetBlobsSize(RNE_BLOBS_S *blobs);

TS_S32 TS_MPI_TRP_RNE_SetCacheAddr(RNE_NET_S *net, size_t axi_addr);

ts_s32 TS_MPI_TRP_RNE_AsyncExecutorBind(RNE_ASYNC_EXECUTOR *async_etr,
                                        RNE_NET_S *nn, ts_s32 priority);

ts_s32 TS_MPI_TRP_RNE_AsyncExecutorUnbind(RNE_ASYNC_EXECUTOR *async_etr);

RNE_BLOBS_S *TS_MPI_TRP_RNE_ForwardAsync(RNE_ASYNC_EXECUTOR *async_etr);

RNE_BLOBS_S *TS_MPI_TRP_RNE_ForwardLowPower(RNE_NET_S *net);

END_DECLS

#endif /* __TS_RNE_C_API_H__ */
