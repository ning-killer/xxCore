#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ts_rne_c_api.h>
#include <ts_rne_log.h>
#include <ts_rne_nn_input.h>
#include <ts_rne_nn_output.h>
#include <ts_rne_time.h>
#include <ts_rne_version.h>

#include "ts_rne_crc.h"
#include "ts_rne_gp_layers.h"

#include "../../model/model3_cfg.h"
#include "../../model/model3_input.h"
#include "../../model/model3_weight.h"

#define modelInput model3_input
#define modelCfg model3_cfg
#define modelWeight model3_weight

static TS_VOID TS_MPI_TRP_RNE_PrintGpLayersList()
{
    RNE_GP_LAYERS_HD_S *layerHd = TS_MPI_TRP_RNE_GetGpLayersHd();
    TS_MPI_TRP_RNE_Info("gpLayerTypeNum: %d\n", layerHd->s32GpLayerTypeNum);
    TS_MPI_TRP_RNE_Info("userLayerTypeNum: %d\n", layerHd->s32UserLayerTypeNum);
    for (TS_S32 i = 0; i < layerHd->s32UserLayerTypeNum; ++i) {
        if (NULL != layerHd->fnpGpLayerHd[i]) {
            TS_MPI_TRP_RNE_Info("user gp layer func: %p\n", layerHd->fnpGpLayerHd[i]);
        }
    }
}

static TS_VOID TS_MPI_TRP_RNE_PrintGpLayersInfoList(RNE_NET_S *net)
{
    RNE_GP_LAYERS_S *layersInfo = TS_MPI_TRP_RNE_GetGpLayers(net);
    if (NULL == layersInfo) {
        TS_MPI_TRP_RNE_Info("layersInfo is NULL\n");
        return;
    }
    TS_MPI_TRP_RNE_Info("gp layers_type sum: %d\n", layersInfo->u32LayersNum);
    for (TS_S32 i = 0; i < layersInfo->u32LayersNum; ++i) {
        TS_MPI_TRP_RNE_Info(
            "layer id: %d layer name: %s layerType:%d typeName:%s\n",
            layersInfo->stpLayerInfo[i].s32LayerId,
            layersInfo->stpLayerInfo[i].cpLayerName,
            layersInfo->stpLayerInfo[i].s32LayerType,
            layersInfo->stpLayerInfo[i].cpTypeName);
    }
}

TS_S32 main(TS_S32 argc, TS_CHAR **argv)
{
    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);
    TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

    TS_MPI_TRP_RNE_Info("current log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    TS_MPI_TRP_RNE_Info("current lib version : %s\n",
                        TS_MPI_TRP_RNE_GetSdkVersion());
    TS_MPI_TRP_RNE_Info("main start...\n");

    RNE_NET_S nnModel;
    memset(&nnModel, 0, sizeof(RNE_NET_S));
    nnModel.u8pGraph = modelCfg;
    nnModel.s32GraphLen = sizeof(modelCfg);
    nnModel.u8pParams = (TS_U8 *)modelWeight;
    nnModel.eInputType = RNE_NET_INPUT_TYPE_INT8_CHW;
    RNE_NET_S *net[] = {&nnModel};
    const TS_S32 num = sizeof(net) / sizeof(net[0]);

    TS_S32 ret = TS_MPI_TRP_RNE_OpenDevice(NULL, TS_MPI_TRP_RNE_RegisterGpLayers);
    if (ret) {
        TS_MPI_TRP_RNE_Error("open device error!\n");
        return ret;
    }

    for (TS_S32 n = 0; n < num; ++n) {
        {
            // layer_id = 4  通过编译器的log可以看到 (-l 0)
            TS_MPI_TRP_RNE_RegisterGpUserData(net[n], 4,
                                              "This is a test for UserData!");
            TS_MPI_TRP_RNE_Info("UserData set ==> %s\n",
                                "This is a test for UserData!");
        }
        ret = TS_MPI_TRP_RNE_LoadModel(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("load model error!\n");
            return ret;
        }
        ret = TS_MPI_TRP_RNE_OnceLoad(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("once load error!\n");
            return ret;
        }
    }

    for (TS_S32 n = 0; n < num; ++n) {
        /* 打印软件层列表*/
        TS_MPI_TRP_RNE_PrintGpLayersList();
        TS_MPI_TRP_RNE_PrintGpLayersInfoList(net[n]);

        if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], 0, modelInput)) {
            TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
            goto releaseResult;
        }
        RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
        if (outputBlobs == NULL) {
            TS_MPI_TRP_RNE_Error("net forward error!\n");
            goto releaseResult;
        }
        TS_SIZE_T sLen = TS_MPI_TRP_RNE_GetBlobsSize(outputBlobs);

        TS_VOID *cpAddr = TS_MPI_TRP_RNE_Alloc(sLen);
        if (NULL == cpAddr) {
            TS_MPI_TRP_RNE_Error("insufficient memory!\n");
            goto releaseResult;
        }
        TS_MPI_TRP_RNE_DumpOutputBlobs(net[n], cpAddr);

        TS_MPI_TRP_RNE_Info("outputBlobs crc16 = %04x\n", RNE_CRC16(cpAddr, sLen));
        TS_MPI_TRP_RNE_Info("outputBlobs crc32 = %08x\n", RNE_CRC32(cpAddr, sLen));
        TS_MPI_TRP_RNE_Info("net %d forward done\n", n);
    }

releaseResult:
    for (TS_S32 n = 0; n < num; ++n) {
        TS_MPI_TRP_RNE_UnloadModel(net[n]);
    }

    TS_MPI_TRP_RNE_CloseDevice();
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
