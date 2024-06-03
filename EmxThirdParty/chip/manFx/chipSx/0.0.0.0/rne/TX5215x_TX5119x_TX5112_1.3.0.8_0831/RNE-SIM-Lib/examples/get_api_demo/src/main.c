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

#include "../../model/model2_cfg.h"
#include "../../model/model2_input.h"
#include "../../model/model2_weight.h"

#define modelInput model2_input
#define modelCfg model2_cfg
#define modelWeight model2_weight

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
        goto releaseResult;
    }

    for (TS_S32 n = 0; n < num; ++n) {
        ret = TS_MPI_TRP_RNE_LoadModel(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("load model error!\n");
            goto releaseResult;
        }
        ret = TS_MPI_TRP_RNE_OnceLoad(net[n]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("once load error!\n");
            goto releaseResult;
        }
    }

    for (TS_S32 n = 0; n < num; ++n) {
        TS_MPI_TRP_RNE_Info("NET(%d) net name      :%s\n", n,
                            TS_MPI_TRP_RNE_GetResVersion(net[n]));
        TS_MPI_TRP_RNE_Info(
            "NET(%d) blob buf size :%lld\n", n,
            (unsigned long long)TS_MPI_TRP_RNE_GetBlobsBufSize(net[n]));
        TS_MPI_TRP_RNE_Info("NET(%d) cfg size      :%lld\n", n,
                            (unsigned long long)TS_MPI_TRP_RNE_GetCfgSize(net[n]));
        TS_MPI_TRP_RNE_Info(
            "NET(%d) weight size   :%lld\n", n,
            (unsigned long long)TS_MPI_TRP_RNE_GetWeightSize(net[n]));
        TS_MPI_TRP_RNE_Info(
            "NET(%d) weight m size :%lld\n", n,
            (unsigned long long)TS_MPI_TRP_RNE_GetWeightMSize(net[n]));
        TS_MPI_TRP_RNE_Info(
            "NET(%d) weight w size :%lld\n", n,
            (unsigned long long)TS_MPI_TRP_RNE_GetWeightWSize(net[n]));
        TS_MPI_TRP_RNE_Info(
            "NET(%d) need mem size :%lld\n", n,
            (unsigned long long)(TS_MPI_TRP_RNE_GetBlobsBufSize(net[n]) +
                                 TS_MPI_TRP_RNE_GetCfgSize(net[n]) +
                                 TS_MPI_TRP_RNE_GetWeightSize(net[n])));

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

        TS_MPI_TRP_RNE_Info("current time: %llu ms\n",
                            TS_MPI_TRP_RNE_GetSysTimeMs());
        TS_MPI_TRP_RNE_Info("current time: %llu us\n",
                            TS_MPI_TRP_RNE_GetSysTimeUs());
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
