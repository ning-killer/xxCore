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

#include "ts_rne_gp_layers.h"
#include "ts_rne_record_file.h"

#include "../../model/model_cfg.h"
#include "../../model/model_input.h"
#include "../../model/model_weight.h"

#define modelInput model_input
#define modelCfg model_cfg
#define modelWeight model_weight

TS_S32 main(TS_S32 argc, TS_CHAR **argv)
{
    /* 1 设置芯片型号
     */
    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);
    /* 2 设置log等级（可选，默认RNE_LOG_ERROR）
     */
    TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

    TS_MPI_TRP_RNE_Info("current log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    TS_MPI_TRP_RNE_Info("current lib version : %s\n",
                        TS_MPI_TRP_RNE_GetSdkVersion());
    TS_MPI_TRP_RNE_Info("main start...\n");

    /* 3 初始化数据结构
     */
    RNE_NET_S nnModel;
    memset(&nnModel, 0, sizeof(RNE_NET_S));
    nnModel.u8pGraph = modelCfg;
    nnModel.s32GraphLen = sizeof(modelCfg);
    nnModel.u8pParams = (TS_U8 *)modelWeight;
    nnModel.eInputType = RNE_NET_INPUT_TYPE_INT8_CHW;
    nnModel.eOutputType = RNE_NET_OUTPUT_TYPE_FORMAT_NONE;
    RNE_NET_S *net[] = {&nnModel};
    const TS_S32 num = sizeof(net) / sizeof(net[0]);

    /* 4 打开RNE设备
     */
    TS_S32 ret = TS_MPI_TRP_RNE_OpenDevice(NULL, TS_MPI_TRP_RNE_RegisterGpLayers);
    if (ret) {
        TS_MPI_TRP_RNE_Error("open device error!\n");
        goto releaseResult;
    }

    /* 5 初始化多网络模型，并在每次初始化网络配置后，进行网络OnceLoad
     */
    for (TS_S32 n = 0; n < num; ++n) {
        /* 初始化单个网路
         */
        ret = TS_MPI_TRP_RNE_LoadModel(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("load model error!\n");
            goto releaseResult;
        }
        /* net once load
         * 仅有网络模型配置为once load情况下，内部才真正执行once load
         */
        ret = TS_MPI_TRP_RNE_OnceLoad(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("once load error!\n");
            goto releaseResult;
        }
    }

    for (TS_S32 n = 0; n < num; ++n) {
        /* 6 填充输入数据
         */
        if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], 0, modelInput)) {
            TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
            goto releaseResult;
        }

        /*  7 网络推理
         */
        RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
        if (outputBlobs == NULL) {
            TS_MPI_TRP_RNE_Error("net forward error!\n");
            goto releaseResult;
        }

        TS_S32 sLen = 0;
        for (TS_S32 index = 0; index < outputBlobs->u32NBlob; ++index) {
            TS_S32 cStride = TS_MPI_TRP_RNE_CStride(outputBlobs->stpBlob[index].s32C,
                                                    outputBlobs->stpBlob[index].s32BitNum,
                                                    outputBlobs->stpBlob[index].bIsJoined);
            sLen += outputBlobs->stpBlob[index].s32N * outputBlobs->stpBlob[index].s32H *
                    outputBlobs->stpBlob[index].s32W * cStride *
                    outputBlobs->stpBlob[index].s32BitNum / CHAR_BIT;
        }

        TS_VOID *cpAddr = TS_MPI_TRP_RNE_Alloc(sLen);
        if (NULL == cpAddr) {
            TS_MPI_TRP_RNE_Error("insufficient memory!\n");
            goto releaseResult;
        }
        /*  8 dump输出数据
         */
        TS_MPI_TRP_RNE_DumpOutputBlobs(net[n], cpAddr);

        RNE_RecordFile(0, 1, outputBlobs, NULL, "./", "int");
        TS_MPI_TRP_RNE_Info("net %d forward done\n", n);

        TS_MPI_TRP_RNE_Free(cpAddr);
    }

releaseResult:
    /* 9 释放网络
     */
    for (TS_S32 n = 0; n < num; ++n) {
        TS_MPI_TRP_RNE_UnloadModel(net[n]);
    }
    /* 10 关闭设备
     */
    TS_MPI_TRP_RNE_CloseDevice();
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
