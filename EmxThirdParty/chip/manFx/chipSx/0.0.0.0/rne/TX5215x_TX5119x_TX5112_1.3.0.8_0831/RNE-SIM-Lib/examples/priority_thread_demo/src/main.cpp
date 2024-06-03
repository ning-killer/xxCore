#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include <ts_rne_log.h>
#include <ts_rne_nn_thread.hpp>
#include <ts_rne_version.h>
#include <ts_rne_nn_input.h>
#include <ts_rne_nn_output.h>
#include "ts_rne_crc.h"
#include "ts_rne_gp_layers.h"


#include "../../model/model2_cfg.h"
#include "../../model/model2_input.h"
#include "../../model/model2_weight.h"
#include "../../model/model_cfg.h"
#include "../../model/model_input.h"
#include "../../model/model_weight.h"

static int init_nets_graph_and_params(RNE_NET_S **net, int n_net, int n_core)
{
    int ret = 0;

    for (int i = 0; i < n_net; i++) {
        uint32_t buf_size = TS_MPI_TRP_RNE_GetBlobsBufSize(net[i]);
        net[i]->vpSelfBuf = TS_MPI_TRP_RNE_AllocLinearMem(buf_size * n_core);
        ret |= !net[i]->vpSelfBuf;
        ret |= TS_MPI_TRP_RNE_LoadModel(net[i]);
        ret |= TS_MPI_TRP_RNE_OnceLoad(net[i]);
    }

    return ret;
}

static void nn_proc(RNE_NET_S *net, int priority, TS_U8 *nn_in, int nn_in_size,
                    int in_size)
{
    RNE_ASYNC_EXECUTOR exe;
    int ret = TS_MPI_TRP_RNE_AsyncExecutorBind(&exe, net, priority);
    if (ret) {
        TS_MPI_TRP_RNE_Error("async_etr %d bind error!\n", priority);
        return;
    }

    RneNNThread *nn_td = RneNNThread::get_instance();
    bool is_exit = false;
    exe.nn_in = nn_in;
    nn_td->push(&exe);

    do {
        RneNNThread::State state = nn_td->query(&exe);
        if (state == RneNNThread::DONE) {
            RNE_BLOBS_S *rst = TS_MPI_TRP_RNE_GetResultBlobs(exe.nn);
            TS_SIZE_T sLen = TS_MPI_TRP_RNE_GetBlobsSize(rst);

            TS_VOID *cpAddr = TS_MPI_TRP_RNE_Alloc(sLen);
            if (NULL == cpAddr) {
                TS_MPI_TRP_RNE_Error("insufficient memory!\n");
                return;
            }
            TS_MPI_TRP_RNE_DumpOutputBlobs(exe.nn, cpAddr);

            TS_MPI_TRP_RNE_Info("outputBlobs crc16 = %04x\n",
                                RNE_CRC16((const TS_U8 *)cpAddr, sLen));
            TS_MPI_TRP_RNE_Info("outputBlobs crc32 = %08x\n",
                                RNE_CRC32((const TS_U8 *)cpAddr, sLen));

            exe.nn_in += nn_in_size;
            if (exe.nn_in + nn_in_size > nn_in + in_size) {
                is_exit = true;
            } else if (!is_exit) {
                nn_td->push(&exe);
            }
        }
    } while (!is_exit);
    TS_MPI_TRP_RNE_AsyncExecutorUnbind(&exe);
}

int main(int argc, char **argv)
{
    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);

    TS_MPI_TRP_RNE_OpenDevice(NULL, TS_MPI_TRP_RNE_RegisterGpLayers);
    int n_core = TS_MPI_TRP_RNE_GetCoreNum();
    TS_MPI_TRP_RNE_Info("default rne core number:%d\n", n_core);

    RNE_NET_S nn_test1;
    memset(&nn_test1, 0, sizeof(nn_test1));
    nn_test1.u8pGraph = model_cfg;
    nn_test1.u8pParams = (uint8_t *)model_weight;
    nn_test1.eInputType = RNE_NET_INPUT_TYPE_INT8_HWC;

    RNE_NET_S nn_test2;
    memset(&nn_test2, 0, sizeof(nn_test2));
    nn_test2.u8pGraph = model2_cfg;
    nn_test2.u8pParams = (uint8_t *)model2_weight;
    nn_test2.eInputType = RNE_NET_INPUT_TYPE_INT8_HWC;

    RNE_NET_S *net[] = {&nn_test1, &nn_test2};
    const int n_net = sizeof(net) / sizeof(net[0]);

    int nn_in_size[n_net] = {0};

    TS_U8 *nn_input[] = {model_input, model2_input};
    RneNNThread *nn_td = RneNNThread::get_instance();
    std::thread t0;
    std::thread t1;

    if (0 != init_nets_graph_and_params(net, n_net, n_core)) {
        TS_MPI_TRP_RNE_Error("nets init error!\n");
        goto release_rsrc;
    }

    for (int j = 0; j < n_net; ++j) {
        RNE_BLOBS_S *in_blobs = TS_MPI_TRP_RNE_GetInputBlobs(net[j]);
        for (uint32_t i = 0; i < in_blobs->u32NBlob; ++i) {
            nn_in_size[j] += in_blobs->stpBlob[i].s32N * in_blobs->stpBlob[i].s32H *
                             in_blobs->stpBlob[i].s32W * in_blobs->stpBlob[i].s32C *
                             (in_blobs->stpBlob[i].s32BitNum / CHAR_BIT);
        }
        TS_MPI_TRP_RNE_FillInputBlobs(net[j], 0, nn_input[j]);
    }

    nn_td->start();

    t0 = std::thread(nn_proc, net[0], 0, nn_input[0], nn_in_size[0],
                     sizeof(model_input));
    t1 = std::thread(nn_proc, net[1], 1, nn_input[1], nn_in_size[1],
                     sizeof(model2_input));

    t0.join();
    t1.join();

    nn_td->end();

release_rsrc:
    for (int n = 0; n < n_net; n++) {
        TS_MPI_TRP_RNE_UnloadModel(net[n]);
        TS_MPI_TRP_RNE_FreeLinearMem(net[n]->vpSelfBuf);
    }
    TS_MPI_TRP_RNE_CloseDevice();
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
