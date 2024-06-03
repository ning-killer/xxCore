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
#include <ts_rne_nn_output.h>
#include <ts_rne_time.h>
#include <ts_rne_version.h>

#include "ts_rne_crc.h"
#include "ts_rne_gp_layers.h"

#define BIN_SUFFIX ".bin"
#define CFG_SUFFIX ".cfg"
#define WEIGHT_SUFFIX ".weight"


TS_S32 RNE_BinFileFilter(const struct dirent *fd)
{
    TS_S32 len = strlen(fd->d_name);
    TS_S32 suffix_len = strlen(BIN_SUFFIX);
    if (!strcmp(BIN_SUFFIX, fd->d_name + len - suffix_len) &&
        fd->d_type != DT_DIR) {
        return 1;
    }
    return 0;
}

TS_S32 RNE_CfgFileFilter(const struct dirent *fd)
{
    TS_S32 len = strlen(fd->d_name);
    TS_S32 suffix_len = strlen(CFG_SUFFIX);
    if (!strcmp(CFG_SUFFIX, fd->d_name + len - suffix_len) &&
        fd->d_type != DT_DIR) {
        return 1;
    }
    return 0;
}

TS_S32 RNE_WeightFileFilter(const struct dirent *fd)
{
    TS_S32 len = strlen(fd->d_name);
    TS_S32 suffix_len = strlen(WEIGHT_SUFFIX);
    if (!strcmp(WEIGHT_SUFFIX, fd->d_name + len - suffix_len) &&
        fd->d_type != DT_DIR) {
        return 1;
    }
    return 0;
}

TS_S32 RNE_GetFileList(const TS_CHAR *type, const TS_CHAR *path,
                       struct dirent ***f_l)
{
    struct dirent **nameList = NULL;
    TS_S32 num = 0;
    if (!strcmp(type, CFG_SUFFIX)) {
        num = scandir(path, &nameList, RNE_CfgFileFilter, versionsort);
        *f_l = nameList;
    } else if (!strcmp(type, WEIGHT_SUFFIX)) {
        num =
            scandir(path, &nameList, RNE_WeightFileFilter, versionsort);
        *f_l = nameList;
    } else if (!strcmp(type, BIN_SUFFIX)) {
        num = scandir(path, &nameList, RNE_BinFileFilter, versionsort);
        *f_l = nameList;
    } else {
        return -1;
    }
    return num;
}

TS_S32 RNE_GetFileSize(const TS_CHAR *path)
{
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

TS_VOID RNE_ReadBinData(TS_CHAR *fileName, uint8_t *buf,
                        TS_S32 size)
{
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

TS_VOID RNE_FreeFileList(TS_S32 num, struct dirent **f_l)
{
    struct dirent **nameList = f_l;
    for (TS_S32 i = 0; i < num; i++) {
        TS_MPI_TRP_RNE_Free(nameList[i]);
        nameList[i] = NULL;
    }
    TS_MPI_TRP_RNE_Free(nameList);
    nameList = NULL;
}

TS_S32 main(TS_S32 argc, TS_CHAR **argv)
{

    struct dirent **input_nameList = NULL;
    struct dirent **weight_nameList = NULL;
    struct dirent **cfg_nameList = NULL;
    TS_S32 inputNum = 0;

    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);

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
    RNE_GetFileList(CFG_SUFFIX, srcDir, &cfg_nameList);

    TS_S32 size =
        strlen(srcDir) + 1 + strlen((TS_CHAR *)cfg_nameList[0]->d_name) + 1;
    TS_CHAR *fileName = TS_MPI_TRP_RNE_Alloc(size);
    strcpy(fileName, srcDir);
    strcat(fileName, "/");
    strcat(fileName, (TS_CHAR *)cfg_nameList[0]->d_name);

    TS_S32 cfgFileSize = RNE_GetFileSize(fileName);

    TS_S32 *cfgData = TS_MPI_TRP_RNE_Alloc(cfgFileSize);
    if (cfgData == NULL) {
        TS_MPI_TRP_RNE_Error("insufficient memory!\n");
        return -1;
    }
    RNE_ReadBinData(fileName, (uint8_t *)cfgData, cfgFileSize);
    if (fileName != NULL) {
        TS_MPI_TRP_RNE_Free(fileName);
    }

    /*get weight file*/
    RNE_GetFileList(WEIGHT_SUFFIX, srcDir, &weight_nameList);
    TS_S32 weight_size =
        strlen(srcDir) + 1 + strlen((TS_CHAR *)weight_nameList[0]->d_name) + 1;
    TS_CHAR *weight_fileName = TS_MPI_TRP_RNE_Alloc(weight_size);
    strcpy(weight_fileName, srcDir);
    strcat(weight_fileName, "/");
    strcat(weight_fileName, (TS_CHAR *)weight_nameList[0]->d_name);
    TS_S32 weightFileSize = RNE_GetFileSize(weight_fileName);

    TS_S32 *weightData = TS_MPI_TRP_RNE_Alloc(weightFileSize);
    if (weightData == NULL) {
        TS_MPI_TRP_RNE_Error("insufficient memory!\n");
        return -1;
    }
    RNE_ReadBinData(weight_fileName, (uint8_t *)weightData,
                    weightFileSize);
    if (weight_fileName != NULL) {
        TS_MPI_TRP_RNE_Free(weight_fileName);
    }

    RNE_NET_S nnModel;
    memset(&nnModel, 0, sizeof(RNE_NET_S));
    nnModel.u8pGraph = (TS_U8 *)cfgData;
    nnModel.s32GraphLen = sizeof(cfgData);
    nnModel.u8pParams = (TS_U8 *)weightData;
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

    inputNum = RNE_GetFileList(BIN_SUFFIX, srcDir, &input_nameList);
    for (TS_S32 n = 0; n < inputNum; ++n) {

        size =
            strlen(srcDir) + 1 + strlen((TS_CHAR *)input_nameList[n]->d_name) + 1;
        TS_CHAR *input_fileName = TS_MPI_TRP_RNE_Alloc(size);
        strcpy(input_fileName, srcDir);
        strcat(input_fileName, "/");
        strcat(input_fileName, (TS_CHAR *)input_nameList[n]->d_name);

        TS_S32 inputFileSize = RNE_GetFileSize(input_fileName);

        TS_S32 *inputData = TS_MPI_TRP_RNE_Alloc(inputFileSize);
        if (inputData == NULL) {
            TS_MPI_TRP_RNE_Error("insufficient memory!\n");
            goto releaseResult;
        }
        RNE_ReadBinData(input_fileName, (uint8_t *)inputData,
                        inputFileSize);
        if (input_fileName != NULL) {
            TS_MPI_TRP_RNE_Free(input_fileName);
        }

        if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[n], 0, inputData)) {
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
    RNE_FreeFileList(inputNum, input_nameList);
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
