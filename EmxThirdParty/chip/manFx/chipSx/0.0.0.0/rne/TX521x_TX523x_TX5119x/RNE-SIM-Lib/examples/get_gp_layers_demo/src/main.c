#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ts_rne_c_api.h>
#include <ts_rne_log.h>
#include <ts_rne_nn_input.h>
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

#define TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM (4)

static TS_S32 TS_MPI_TRP_RNE_InitNetsGraphAndParams(RNE_NET_S **net,
                                                    TS_U8 **paramStride,
                                                    TS_S32 *paramSize,
                                                    TS_S32 nNet) {
  /* 3 打开RNE设备
   */
  TS_S32 ret = TS_MPI_TRP_RNE_OpenDevice(NULL, TS_MPI_TRP_RNE_RegisterGpLayers);
  if (ret) {
    TS_MPI_TRP_RNE_Error("open device error!\n");
    return ret;
  }
  /* 4 初始化多网络模型，并在每次初始化网络配置后，进行网络OnceLoad
   */
  for (TS_S32 i = 0; i < nNet; ++i) {
    /* 量化和权重数据需要4byte对齐
     * 如果未在头文件4byte对齐，可执行W_ALIGN_BYTES_NUM内代码，进行4字节对齐
     */
#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
    if (((TS_SIZE_T)net[i]->u8pParams &
         (TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1)) != 0) {
      paramStride[i] = (TS_U8 *)TS_MPI_TRP_RNE_Alloc(
          paramSize[i] + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
      if (NULL == paramStride[i]) {
        TS_MPI_TRP_RNE_Error("insufficient memory!\n");
        return ret;
      }
      TS_SIZE_T addr = (TS_SIZE_T)paramStride[i];
      addr += TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1;
      addr &= ~(TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1);
      memcpy((TS_VOID *)addr, net[i]->u8pParams, paramSize[i]);
      net[i]->u8pParams = (TS_U8 *)addr;
    }
#endif
    {
      // layer_id = 4  通过编译器的log可以看到 (-l 0)
      TS_MPI_TRP_RNE_RegisterGpUserData(net[i], 4,
                                        "This is a test for UserData!");
      TS_MPI_TRP_RNE_Info("UserData set ==> %s\n",
                          "This is a test for UserData!");
    }
    /* 初始化单个网路
     */
    ret = TS_MPI_TRP_RNE_LoadModel(net[i]);
    if (ret) {
      TS_MPI_TRP_RNE_Error("load model error!\n");
      return ret;
    }
    /* net once load
     * 仅有网络模型配置为once load情况下，内部才真正执行once load
     */
    ret = TS_MPI_TRP_RNE_OnceLoad(net[i]);
    if (ret) {
      TS_MPI_TRP_RNE_Error("once load error!\n");
      return ret;
    }
  }

  return ret;
}

static TS_VOID TS_MPI_TRP_RNE_PrintGpLayersList() {
  RNE_GP_LAYERS_HD_S *layerHd = TS_MPI_TRP_RNE_GetGpLayersHd();
  TS_MPI_TRP_RNE_Info("gpLayerTypeNum: %d\n", layerHd->s32GpLayerTypeNum);
  TS_MPI_TRP_RNE_Info("userLayerTypeNum: %d\n", layerHd->s32UserLayerTypeNum);
  for (TS_S32 i = 0; i < layerHd->s32UserLayerTypeNum; ++i) {
    if (NULL != layerHd->fnpGpLayerHd[i]) {
      TS_MPI_TRP_RNE_Info("user gp layer func: %p\n", layerHd->fnpGpLayerHd[i]);
    }
  }
}

static TS_VOID TS_MPI_TRP_RNE_PrintGpLayersInfoList(RNE_NET_S *net) {
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

TS_S32 main(TS_S32 argc, TS_CHAR **argv) {
  TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);
  /* 1 设置log等级
   */
  TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

  TS_MPI_TRP_RNE_Info("current log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
  TS_MPI_TRP_RNE_Info("current lib version : %s\n",
                      TS_MPI_TRP_RNE_GetSdkVersion());
  TS_MPI_TRP_RNE_Info("main start...\n");

  /* 2 初始化数据结构
   */
  RNE_NET_S nnModel;
  memset(&nnModel, 0, sizeof(RNE_NET_S));
  nnModel.u8pGraph = modelCfg;
  nnModel.s32GraphLen = sizeof(modelCfg);
  nnModel.u8pParams = (TS_U8 *)modelWeight;
  nnModel.eInputType = RNE_NET_INPUT_TYPE_INT8_CHW;
  RNE_NET_S *net[] = {&nnModel};
  const TS_S32 num = sizeof(net) / sizeof(net[0]);
  TS_S32 paramSize[] = {sizeof(modelWeight)};
  TS_U8 *paramStride[num];
  memset(paramStride, 0, sizeof(paramStride));
  if (0 !=
      TS_MPI_TRP_RNE_InitNetsGraphAndParams(net, paramStride, paramSize, num)) {
    TS_MPI_TRP_RNE_Error("nets init error!\n");
    goto releaseResult;
  }

  /* 5 进行网络推理并获取网络推理结果
   */
  for (TS_S32 n = 0; n < num; ++n) {
    /* 打印软件层列表*/
    TS_MPI_TRP_RNE_PrintGpLayersList();
    TS_MPI_TRP_RNE_PrintGpLayersInfoList(net[n]);

    RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(net[n]);
    if (NULL == inputBlobs) {
      TS_MPI_TRP_RNE_Error("inputBlobs is NULL!\n");
      goto releaseResult;
    }
    if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], inputBlobs, 0, modelInput)) {
      TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
      goto releaseResult;
    }
    RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
    if (outputBlobs == NULL) {
      TS_MPI_TRP_RNE_Error("net forward error!\n");
      goto releaseResult;
    }
    TS_MPI_TRP_RNE_Info("net %d forward done\n", n);
    TS_MPI_TRP_RNE_Info("finial crc = %04x\n",
                        TS_MPI_TRP_RNE_GetBlobsCRC16CCITT(outputBlobs));
  }

releaseResult:
  /* 6 释放网络
   */
  for (TS_S32 n = 0; n < num; ++n) {
    TS_MPI_TRP_RNE_Free(paramStride[n]);
    TS_MPI_TRP_RNE_UnloadModel(net[n]);
  }
  /* 7 关闭设备
   */
  TS_MPI_TRP_RNE_CloseDevice();
  TS_MPI_TRP_RNE_Info("program finished...\n");
  return 0;
}
