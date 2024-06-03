#include "ts_rne_record_file.h"

#include <stdio.h>
#include <string.h>
#include <ts_rne_log.h>

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef MASK_BITS
#define MASK_BITS(m) ((1ll << (m)) - 1)
#endif

#define THIS_IS_SAME_CODE                                                                                     \
    memset(debugLayerNamef, 0, sizeof(debugLayerNamef));                                                      \
    TS_S32 uSize = debug->stpBlob[idx].s32BitNum / CHAR_BIT;                                                  \
    TS_S32 c = debug->stpBlob[idx].s32C;                                                                      \
    TS_S32 cStride = TS_MPI_TRP_RNE_CStride(c, debug->stpBlob[idx].s32BitNum, debug->stpBlob[idx].bIsJoined); \
    TS_S32 h = debug->stpBlob[idx].s32H;                                                                      \
    TS_S32 w = debug->stpBlob[idx].s32W;                                                                      \
    TS_S32 q_rst = *(debug->stpBlob[idx].s32pQ);                                                              \
    TS_S32 num = debug->stpBlob[idx].s32N;                                                                    \
    bool is_unsign = debug->stpBlob[idx].bIsUnsigned;                                                         \
    float coeff = *(debug->stpBlob[idx].fCoeff);                                                              \
    uint8_t *out = (uint8_t *)(debug->stpBlob[idx].vpAddr);                                                   \
    FILE *file = NULL;

#define THIS_IS_SAME_CODE_PRINT                                           \
    if (frameIndex == (frameCount - 1)) {                                 \
        TS_MPI_TRP_RNE_Info("Write file success: %s\n", debugLayerNamef); \
    }

TS_VOID TS_MPI_TRP_RNE_ReplaceCToC(TS_CHAR *str, TS_CHAR frontC, TS_CHAR afterC)
{
    TS_CHAR *ptr = str;
    while (*ptr != '\0') {
        if (*ptr == frontC) *ptr = afterC;
        ptr++;
    }
}

static TS_CHAR debugLayerNamef[1024];
TS_S32 TS_MPI_TRP_RNE_RecordHwcPFile(TS_S32 frameIndex, TS_S32 frameCount,
                                     RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                                     TS_CHAR *outputPath, TS_CHAR *debugName, TS_S32 outType)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        THIS_IS_SAME_CODE
        sprintf(debugLayerNamef, "%s%s_%d_hwc_p.txt", outputPath, debugName, idx);
        if (frameIndex == 0) {
            file = fopen(debugLayerNamef, "w");
        } else {
            file = fopen(debugLayerNamef, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", debugLayerNamef);
            continue;
        }
#ifndef DISABLE_PRINT_DEBUG_LAYER
        printf("SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", num, h, w, c);
#endif
        fprintf(file, "SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", num, h, w, c);
        float *out_f = (float *)out;
        for (TS_S32 n = 0; n < num; n++) {
            for (TS_S32 i = 0; i < h; i++) {
                for (TS_S32 j = 0; j < w; j++) {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("HW[%d,%d]:\n", i, j);
#endif
                    fprintf(file, "HW[%d,%d]:\n", i, j);
                    for (TS_S32 k = 0; k < c; k++) {
                        if (q_rst >= 0) {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                            if (!outType) {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if ((!is_unsign) && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%0d,", data);
#endif
                                fprintf(file, "%0d,", data);
                            } else {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%f,", data * (coeff));
#endif
                                fprintf(file, "%f,", data * (coeff));
                            }
                        } else {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                            printf("%f,", out_f[(j + i * w) * cStride + k]);
#endif
                            fprintf(file, "%f,", out_f[(j + i * w) * cStride + k]);
                        }
                    }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("\n");
#endif
                    fprintf(file, "\n");
                }
            }
            out += h * w * cStride * uSize;
            out_f += h * w * cStride;
        }
        fclose(file);
        THIS_IS_SAME_CODE_PRINT
    }
    return 0;
}

TS_S32 TS_MPI_TRP_RNE_RecordHwcFile(TS_S32 frameIndex, TS_S32 frameCount,
                                    RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                                    TS_CHAR *outputPath, TS_CHAR *debugName, TS_S32 outType)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        THIS_IS_SAME_CODE
        sprintf(debugLayerNamef, "%s%s_%d_hwc.txt", outputPath, debugName, idx);
        if (frameIndex == 0) {
            file = fopen(debugLayerNamef, "w");
        } else {
            file = fopen(debugLayerNamef, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", debugLayerNamef);
            continue;
        }
#ifndef DISABLE_PRINT_DEBUG_LAYER
        printf("SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", num, h, w, c);
#endif
        fprintf(file, "SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", num, h, w, c);
        float *out_f = (float *)out;
        for (TS_S32 n = 0; n < num; n++) {
            for (TS_S32 i = 0; i < h; i++) {
                for (TS_S32 j = 0; j < w; j++) {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("HW[%d,%d]:\n", i, j);
#endif
                    fprintf(file, "HW[%d,%d]:\n", i, j);
                    for (TS_S32 k = 0; k < c; k++) {
                        if (q_rst >= 0) {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                            if (!outType) {
                                TS_CHAR format[32];
                                memset(format, 0, sizeof(format));
                                TS_S32 bitNum = debug->stpBlob[idx].s32BitNum;
                                sprintf(format, "%%0%dx,", bitNum / 4);
                                TS_S32 dataEnd = data & MASK_BITS(bitNum);
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf(format, dataEnd);
#endif
                                fprintf(file, format, dataEnd);
                            } else {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
                                float f_data = data * coeff;
                                TS_U32 *t_data = (TS_U32 *)&f_data;
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%08x,", *t_data);
#endif
                                fprintf(file, "%08x,", *t_data);
                            }
                        } else {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                            {
                                TS_CHAR format[32];
                                memset(format, 0, sizeof(format));
                                TS_S32 bitNum = debug->stpBlob[idx].s32BitNum;
                                sprintf(format, "%%0%dx,", bitNum / 4);
                                TS_S32 dataEnd = data & MASK_BITS(bitNum);
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf(format, dataEnd);
#endif
                                fprintf(file, format, dataEnd);
                            }
                        }
                    }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("\n");
#endif
                    fprintf(file, "\n");
                }
            }
            out += h * w * cStride * uSize;
            out_f += h * w * cStride;
        }
        fclose(file);
        THIS_IS_SAME_CODE_PRINT
    }
    return 0;
}

TS_S32 TS_MPI_TRP_RNE_RecordChwPFile(TS_S32 frameIndex, TS_S32 frameCount,
                                     RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                                     TS_CHAR *outputPath, TS_CHAR *debugName, TS_S32 outType)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        THIS_IS_SAME_CODE
        sprintf(debugLayerNamef, "%s%s_%d_p.txt", outputPath, debugName, idx);
        if (frameIndex == 0) {
            file = fopen(debugLayerNamef, "w");
        } else {
            file = fopen(debugLayerNamef, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", debugLayerNamef);
            continue;
        }
#ifndef DISABLE_PRINT_DEBUG_LAYER
        printf("SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", num, c, h, w);
#endif
        fprintf(file, "SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", num, c, h, w);
        float *out_f = (float *)out;
        for (TS_S32 n = 0; n < num; n++) {
            for (TS_S32 k = 0; k < c; k++) {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                printf("C[%d]:\n", k);
#endif
                fprintf(file, "C[%d]:\n", k);
                for (TS_S32 i = 0; i < h; i++) {
                    for (TS_S32 j = 0; j < w; j++) {
                        if (q_rst >= 0) {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                            if (!outType) {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if ((!is_unsign) && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%0d,", data);
#endif
                                fprintf(file, "%0d,", data);
                            } else {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%f,", data * (coeff));
#endif
                                fprintf(file, "%f,", data * (coeff));
                            }
                        } else {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                            printf("%f,", out_f[(j + i * w) * cStride + k]);
#endif
                            fprintf(file, "%f,", out_f[(j + i * w) * cStride + k]);
                        }
                    }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("\n");
#endif
                    fprintf(file, "\n");
                }
            }
            out += h * w * cStride * uSize;
            out_f += h * w * cStride;
        }
        fclose(file);
        THIS_IS_SAME_CODE_PRINT
    }
    return 0;
}

TS_S32 TS_MPI_TRP_RNE_RecordChwFile(TS_S32 frameIndex, TS_S32 frameCount,
                                    RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                                    TS_CHAR *outputPath, TS_CHAR *debugName, TS_S32 outType)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        THIS_IS_SAME_CODE
        sprintf(debugLayerNamef, "%s%s_%d.txt", outputPath, debugName, idx);
        if (frameIndex == 0) {
            file = fopen(debugLayerNamef, "w");
        } else {
            file = fopen(debugLayerNamef, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", debugLayerNamef);
            continue;
        }
#ifndef DISABLE_PRINT_DEBUG_LAYER
        printf("SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", num, c, h, w);
#endif
        fprintf(file, "SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", num, c, h, w);
        float *out_f = (float *)out;
        for (TS_S32 n = 0; n < num; n++) {
            for (TS_S32 k = 0; k < c; k++) {
#ifndef DISABLE_PRINT_DEBUG_LAYER
                printf("C[%d]:\n", k);
#endif
                fprintf(file, "C[%d]:\n", k);
                for (TS_S32 i = 0; i < h; i++) {
                    for (TS_S32 j = 0; j < w; j++) {
                        if (q_rst >= 0) {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);

                            if (!outType) {
                                TS_CHAR format[32];
                                memset(format, 0, sizeof(format));
                                TS_S32 bitNum = debug->stpBlob[idx].s32BitNum;
                                sprintf(format, "%%0%dx,", bitNum / 4);
                                TS_S32 dataEnd = data & MASK_BITS(bitNum);
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf(format, dataEnd);
#endif
                                fprintf(file, format, dataEnd);
                            } else {
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }
                                float f_data = data * coeff;
                                TS_U32 *t_data = (TS_U32 *)&f_data;
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf("%08x,", *t_data);
#endif
                                fprintf(file, "%08x,", *t_data);
                            }
                        } else {
                            TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + k) * uSize));
                            TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                            {
                                TS_CHAR format[32];
                                memset(format, 0, sizeof(format));
                                TS_S32 bitNum = debug->stpBlob[idx].s32BitNum;
                                sprintf(format, "%%0%dx,", bitNum / 4);
                                TS_S32 dataEnd = data & MASK_BITS(bitNum);
#ifndef DISABLE_PRINT_DEBUG_LAYER
                                printf(format, dataEnd);
#endif
                                fprintf(file, format, dataEnd);
                            }
                        }
                    }
#ifndef DISABLE_PRINT_DEBUG_LAYER
                    printf("\n");
#endif
                    fprintf(file, "\n");
                }
            }
            out += h * w * cStride * uSize;
            out_f += h * w * cStride;
        }
        fclose(file);
        THIS_IS_SAME_CODE_PRINT
    }
    return 0;
}

TS_S32 RNE_RecordFile(TS_S32 frameIndex, TS_S32 frameCount,
                      RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                      TS_CHAR *outputPath, const TS_CHAR *type)
{
    if (debug == NULL) return -1;
#if defined(RESULT_HWC_P) || defined(RESULT_HWC) || defined(RESULT_CHW_P) || defined(RESULT_CHW) || defined(RESULT_ALL)
    TS_CHAR result[] = "result";
    TS_CHAR *debugName = debugLayerName ? debugLayerName : result;
    TS_MPI_TRP_RNE_ReplaceCToC(debugName, '/', '_');
    TS_S32 outType = 0;
    if (!strcmp(type, "int")) {
        outType = 0;
    } else if (!strcmp(type, "float")) {
        outType = 1;
    }
#endif

#if defined(RESULT_ALL)
    TS_MPI_TRP_RNE_RecordHwcPFile(frameIndex, frameCount, debug, debugLayerName,
                                  outputPath, debugName, outType);
    TS_MPI_TRP_RNE_RecordHwcFile(frameIndex, frameCount, debug, debugLayerName,
                                 outputPath, debugName, outType);
    TS_MPI_TRP_RNE_RecordChwPFile(frameIndex, frameCount, debug, debugLayerName,
                                  outputPath, debugName, outType);
    TS_MPI_TRP_RNE_RecordChwFile(frameIndex, frameCount, debug, debugLayerName,
                                 outputPath, debugName, outType);
#elif defined(RESULT_HWC_P)
    TS_MPI_TRP_RNE_RecordHwcPFile(frameIndex, frameCount, debug, debugLayerName,
                                  outputPath, debugName, outType);
#elif defined(RESULT_HWC)
    TS_MPI_TRP_RNE_RecordHwcFile(frameIndex, frameCount, debug, debugLayerName,
                                 outputPath, debugName, outType);
#elif defined(RESULT_CHW_P)
    TS_MPI_TRP_RNE_RecordChwPFile(frameIndex, frameCount, debug, debugLayerName,
                                  outputPath, debugName, outType);
#elif defined(RESULT_CHW)
    TS_MPI_TRP_RNE_RecordChwFile(frameIndex, frameCount, debug, debugLayerName,
                                 outputPath, debugName, outType);
#endif
    return 0;
}
