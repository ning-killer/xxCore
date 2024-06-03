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

        if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], 0, modelInput)) {
            TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
            goto releaseResult;
        }

        RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
        if (outputBlobs == NULL) {
            TS_MPI_TRP_RNE_Error("net forward error!\n");
            goto releaseResult;
        }
        TS_MPI_TRP_RNE_Info("net %d forward done\n", n);
        TS_MPI_TRP_RNE_Info("outputBlobs crc = %04x\n",
                            RNE_GetBlobsCRC16CCITT(outputBlobs));
    }

releaseResult:
    for (TS_S32 n = 0; n < num; ++n) {
        TS_MPI_TRP_RNE_UnloadModel(net[n]);
    }

    TS_MPI_TRP_RNE_CloseDevice();
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
