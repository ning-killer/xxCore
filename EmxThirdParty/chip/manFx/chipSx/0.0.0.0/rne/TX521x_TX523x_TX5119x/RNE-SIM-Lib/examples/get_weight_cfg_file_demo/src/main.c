#define _GNU_SOURCE
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <ts_rne_c_api.h>
#include <ts_rne_log.h>
#include <ts_rne_nn_input.h>
#include <ts_rne_time.h>
#include <ts_rne_version.h>

#include "ts_rne_crc.h"
#include "ts_rne_gp_layers.h"

#define BIN_SUFFIX ".bin"
#define CFG_SUFFIX ".cfg"
#define WEIGHT_SUFFIX ".weight"
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

TS_S32 TS_MPI_TRP_RNE_BinFileFilter(const struct dirent *fd) {
  TS_S32 len = strlen(fd->d_name);
  TS_S32 suffix_len = strlen(BIN_SUFFIX);
  if (!strcmp(BIN_SUFFIX, fd->d_name + len - suffix_len) &&
      fd->d_type != DT_DIR) {
    return 1;
  }
  return 0;
}

TS_S32 TS_MPI_TRP_RNE_CfgFileFilter(const struct dirent *fd) {
  TS_S32 len = strlen(fd->d_name);
  TS_S32 suffix_len = strlen(CFG_SUFFIX);
  if (!strcmp(CFG_SUFFIX, fd->d_name + len - suffix_len) &&
      fd->d_type != DT_DIR) {
    return 1;
  }
  return 0;
}

TS_S32 TS_MPI_TRP_RNE_WeightFileFilter(const struct dirent *fd) {
  TS_S32 len = strlen(fd->d_name);
  TS_S32 suffix_len = strlen(WEIGHT_SUFFIX);
  if (!strcmp(WEIGHT_SUFFIX, fd->d_name + len - suffix_len) &&
      fd->d_type != DT_DIR) {
    return 1;
  }
  return 0;
}

TS_S32 TS_MPI_TRP_RNE_GetFileList(const TS_CHAR *type, const TS_CHAR *path,
                                  struct dirent ***f_l) {
  struct dirent **nameList = NULL;
  TS_S32 num = 0;
  if (!strcmp(type, CFG_SUFFIX)) {
    num = scandir(path, &nameList, TS_MPI_TRP_RNE_CfgFileFilter, versionsort);
    *f_l = nameList;
  } else if (!strcmp(type, WEIGHT_SUFFIX)) {
    num =
        scandir(path, &nameList, TS_MPI_TRP_RNE_WeightFileFilter, versionsort);
    *f_l = nameList;
  } else if (!strcmp(type, BIN_SUFFIX)) {
    num = scandir(path, &nameList, TS_MPI_TRP_RNE_BinFileFilter, versionsort);
    *f_l = nameList;
  } else {
    return -1;
  }
  return num;
}

TS_S32 TS_MPI_TRP_RNE_GetFileSize(const TS_CHAR *path) {
  TS_S32 fileSize = -1;

  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    TS_MPI_TRP_RNE_Error("open file failed:%s\n", path);
    return fileSize;
  }
  TS_S32 rc = fseek(file, 0, SEEK_END);
  if (rc) {
    TS_MPI_TRP_RNE_Error("fseek end failed\n");
    return fileSize;
  }

  fileSize = ftell(file);

  rc = fclose(file);
  if (rc) {
    TS_MPI_TRP_RNE_Error("file close failed\n");
    return fileSize;
  }
  return fileSize;
}

TS_VOID TS_MPI_TRP_RNE_ReadBinData(TS_CHAR *fileName, uint8_t *buf,
                                   TS_S32 size) {
  FILE *file = NULL;
  TS_S32 rc = 0;

  file = fopen(fileName, "rb");
  if (file == NULL) {
    TS_MPI_TRP_RNE_Error("fopen:%s\n", fileName);
  }

  rc = fseek(file, 0, SEEK_SET);
  if (rc) {
    TS_MPI_TRP_RNE_Error("fseek\n");
  }

  rc = fread(buf, sizeof(uint8_t), size, file);

  rc = fclose(file);
  if (rc) {
    TS_MPI_TRP_RNE_Error("fclose\n");
  }
}

TS_VOID TS_MPI_TRP_RNE_FreeFileList(TS_S32 num, struct dirent **f_l) {
  struct dirent **nameList = f_l;
  for (TS_S32 i = 0; i < num; i++) {
    TS_MPI_TRP_RNE_Free(nameList[i]);
    nameList[i] = NULL;
  }
  TS_MPI_TRP_RNE_Free(nameList);
  nameList = NULL;
}

TS_S32 main(TS_S32 argc, TS_CHAR **argv) {

  struct dirent **input_nameList = NULL;
  struct dirent **weight_nameList = NULL;
  struct dirent **cfg_nameList = NULL;
  TS_S32 inputNum = 0;

  TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX232);
  /* 1 设置log等级
   */
  TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

  TS_MPI_TRP_RNE_Info("current log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
  TS_MPI_TRP_RNE_Info("current lib version : %s\n",
                      TS_MPI_TRP_RNE_GetSdkVersion());
  TS_MPI_TRP_RNE_Info("main start...\n");

  if (argc < 3) {
    TS_MPI_TRP_RNE_Info("simple_forward_demo <src_dir> <dst_dir>\n");
    return -1;
  }
  TS_CHAR *srcDir = argv[1];

  /*get cfg file*/
  TS_MPI_TRP_RNE_GetFileList(CFG_SUFFIX, srcDir, &cfg_nameList);

  TS_S32 size =
      strlen(srcDir) + 1 + strlen((TS_CHAR *)cfg_nameList[0]->d_name) + 1;
  TS_CHAR *fileName = TS_MPI_TRP_RNE_Alloc(size);
  strcpy(fileName, srcDir);
  strcat(fileName, "/");
  strcat(fileName, (TS_CHAR *)cfg_nameList[0]->d_name);

  TS_S32 cfgFileSize = TS_MPI_TRP_RNE_GetFileSize(fileName);

  TS_S32 *cfgData = TS_MPI_TRP_RNE_Alloc(cfgFileSize);
  if (cfgData == NULL) {
    TS_MPI_TRP_RNE_Error("insufficient memory!\n");
    // goto releaseResult;
  }
  TS_MPI_TRP_RNE_ReadBinData(fileName, (uint8_t *)cfgData, cfgFileSize);
  if (fileName != NULL) {
    TS_MPI_TRP_RNE_Free(fileName);
  }

  /*get weight file*/
  TS_MPI_TRP_RNE_GetFileList(WEIGHT_SUFFIX, srcDir, &weight_nameList);
  TS_S32 weight_size =
      strlen(srcDir) + 1 + strlen((TS_CHAR *)weight_nameList[0]->d_name) + 1;
  TS_CHAR *weight_fileName = TS_MPI_TRP_RNE_Alloc(weight_size);
  strcpy(weight_fileName, srcDir);
  strcat(weight_fileName, "/");
  strcat(weight_fileName, (TS_CHAR *)weight_nameList[0]->d_name);
  TS_S32 weightFileSize = TS_MPI_TRP_RNE_GetFileSize(weight_fileName);

  TS_S32 *weightData = TS_MPI_TRP_RNE_Alloc(weightFileSize);
  if (weightData == NULL) {
    TS_MPI_TRP_RNE_Error("insufficient memory!\n");
    // goto releaseResult;
  }
  TS_MPI_TRP_RNE_ReadBinData(weight_fileName, (uint8_t *)weightData,
                             weightFileSize);
  if (weight_fileName != NULL) {
    TS_MPI_TRP_RNE_Free(weight_fileName);
  }
  /* 2 初始化数据结构
   */
  RNE_NET_S nnModel;
  memset(&nnModel, 0, sizeof(RNE_NET_S));
  nnModel.u8pGraph = (TS_U8 *)cfgData;
  nnModel.s32GraphLen = sizeof(cfgData);
  nnModel.u8pParams = (TS_U8 *)weightData;
  nnModel.eInputType = RNE_NET_INPUT_TYPE_INT8_CHW;
  RNE_NET_S *net[] = {&nnModel};
  const TS_S32 num = sizeof(net) / sizeof(net[0]);
  TS_S32 paramSize[] = {sizeof(weightData)};
  TS_U8 *paramStride[num];
  memset(paramStride, 0, sizeof(paramStride));
  if (0 !=
      TS_MPI_TRP_RNE_InitNetsGraphAndParams(net, paramStride, paramSize, num)) {
    TS_MPI_TRP_RNE_Error("nets init error!\n");
    goto releaseResult;
  }

  /* 5 进行网络推理并获取网络推理结果
   */
  inputNum = TS_MPI_TRP_RNE_GetFileList(BIN_SUFFIX, srcDir, &input_nameList);
  for (TS_S32 n = 0; n < inputNum; ++n) {

    size =
        strlen(srcDir) + 1 + strlen((TS_CHAR *)input_nameList[n]->d_name) + 1;
    TS_CHAR *input_fileName = TS_MPI_TRP_RNE_Alloc(size);
    strcpy(input_fileName, srcDir);
    strcat(input_fileName, "/");
    strcat(input_fileName, (TS_CHAR *)input_nameList[n]->d_name);

    TS_S32 inputFileSize = TS_MPI_TRP_RNE_GetFileSize(input_fileName);

    TS_S32 *inputData = TS_MPI_TRP_RNE_Alloc(inputFileSize);
    if (inputData == NULL) {
      TS_MPI_TRP_RNE_Error("insufficient memory!\n");
      goto releaseResult;
    }
    TS_MPI_TRP_RNE_ReadBinData(input_fileName, (uint8_t *)inputData,
                               inputFileSize);
    if (input_fileName != NULL) {
      TS_MPI_TRP_RNE_Free(input_fileName);
    }

    RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(net[n]);
    if (NULL == inputBlobs) {
      TS_MPI_TRP_RNE_Error("inputBlobs is NULL!\n");
      goto releaseResult;
    }
    if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], inputBlobs, 0, inputData)) {
      TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
      goto releaseResult;
    }
    RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
    if (outputBlobs == NULL) {
      TS_MPI_TRP_RNE_Error("net forward error!\n");
      goto releaseResult;
    }
    TS_S32 crc = TS_MPI_TRP_RNE_GetBlobsCRC16CCITT(outputBlobs);
    TS_MPI_TRP_RNE_Info("outputBlobs crc = %04x\n", crc);
    TS_MPI_TRP_RNE_Info("net %d forward done\n", n);
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
  // if (nameList != NULL) {
  //     TS_MPI_TRP_RNE_FreeFileList(inputNum,nameList);
  // }
  return 0;
}
