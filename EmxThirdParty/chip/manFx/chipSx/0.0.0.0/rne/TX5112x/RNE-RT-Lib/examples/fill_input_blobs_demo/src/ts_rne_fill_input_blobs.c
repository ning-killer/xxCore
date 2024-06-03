#include "ts_rne_log.h"
#include "ts_rne_nn_input.h"
#include <string.h>

#ifndef EPSILON
#define EPSILON 0.000001
#endif

static TS_BOOL TS_MPI_TRP_RNE_InputBlobIsFloat(RNE_BLOB_S *blob)
{
    if ((blob->fCoeff != NULL &&
         (((*blob->fCoeff - 1) < (-EPSILON)) || ((*blob->fCoeff - 1) > EPSILON))) ||
        (*blob->s32pQ == -1)) {
        return true;
    } else {
        return false;
    }
}

#ifndef MIN_BIT
#define MIN_BIT(n) (((1ull << (64 - (n))) - 1) << (n))
#endif
#ifndef MASK_BITS
#define MASK_BITS(m) ((1ll << (m)) - 1)
#endif
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#ifndef BLOB_ALIGN_BYTES
#define BLOB_ALIGN_BYTES (32 / 8)
#endif

TS_S32 TS_MPI_TRP_RNE_FillInputBlobs(RNE_NET_S *net, RNE_BLOBS_S *blobs,
                                     TS_S32 coreIdx, TS_VOID *input)
{
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_FillInputBlobs is open!\n");
    TS_U8 *src_ptr = (TS_U8 *)(input);
    for (uint32_t idx_blob = 0; idx_blob < blobs->u32NBlob; ++idx_blob) {
        RNE_BLOB_S *blob = &blobs->stpBlob[idx_blob];
        TS_S32 n = blob->s32N;
        TS_S32 w = blob->s32W;
        TS_S32 h = blob->s32H;
        TS_S32 c = blob->s32C;
        TS_S32 q_scale = *blob->s32pQ;
        TS_S32 scale_int = *blob->fCoeff * (1LL << q_scale);
        TS_S32 c_stride =
            TS_MPI_TRP_RNE_CStride(blob->s32C, blob->s32BitNum, blob->bIsJoined);
        if (blob->eFormat == RNE_BLOB_N_H_W_C) {
            c_stride = c;
        }
        TS_S32 unit_size = blob->s32BitNum / CHAR_BIT;
        TS_S32 bits = blob->bIsUnsigned ? blob->s32BitNum + 1 : blob->s32BitNum;
        TS_S64 data_max = ~MIN_BIT(bits - 1);
        TS_S32 sign_bit = sizeof(TS_S64) * CHAR_BIT - blob->s32BitNum;
        TS_S64 data_min = ((TS_S64)(MIN_BIT(bits - 1) & MASK_BITS(blob->s32BitNum))
                           << sign_bit) >>
                          sign_bit;
        TS_S64 scale_max = ~MIN_BIT(16 - 1);
        TS_S64 scale_min = MIN_BIT(16 - 1);
        TS_S64 scale64_max = ~MIN_BIT(64 - 1);
        TS_S64 scale64_min = MIN_BIT(64 - 1);
        scale_int = scale_int > scale_max ? scale_max : scale_int;
        scale_int = scale_int < scale_min ? scale_min : scale_int;

        TS_U8 *dst = (TS_U8 *)(blob->vpAddr);
        TS_S32 in_type = net->eInputType;

        if (in_type == RNE_NET_INPUT_TYPE_FLOAT32_HWC &&
            !TS_MPI_TRP_RNE_InputBlobIsFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT8_HWC;
        } else if (in_type == RNE_NET_INPUT_TYPE_FLOAT32_CHW &&
                   !TS_MPI_TRP_RNE_InputBlobIsFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT8_CHW;
        }

        switch (in_type) {
        case RNE_NET_INPUT_TYPE_FLOAT32_HWC: {
            TS_FLOAT *src = (TS_FLOAT *)src_ptr;
            if (q_scale == -1) {
                memcpy(dst, src, n * h * w * c * unit_size);
            } else {
                for (TS_S32 bat = 0; bat < n; bat++) {
                    for (TS_S32 i = 0; i < h * w; ++i) {
                        for (TS_S32 k = 0; k < c; ++k) {
                            TS_DOUBLE tmp = (TS_DOUBLE)(*src++) * scale_int;
                            tmp = tmp > scale64_max ? scale64_max : tmp;
                            tmp = tmp < scale64_min ? scale64_min : tmp;
                            TS_S64 data = 0;
                            if (blob->s32BitNum == 8) {
                                tmp /= (1 << q_scale);
                                data = ((((TS_S64)(tmp * (1 << q_scale)) | (1LL << 62)) >> 62) *
                                            0.5f +
                                        tmp);
                            } else {
                                data = ((TS_S64)(tmp)) >> q_scale;
                            }
                            data = data > data_max ? data_max : data;
                            data = data < data_min ? data_min : data;

                            for (TS_S32 u = 0; u < unit_size; ++u) {
                                dst[k * unit_size + u] = data >> (u * CHAR_BIT);
                            }
                        }
                        dst += c_stride * unit_size;
                    }
                    dst = (TS_U8 *)(((size_t)dst + BLOB_ALIGN_BYTES - 1) &
                                    (~((size_t)BLOB_ALIGN_BYTES - 1)));
                }
            }
            src_ptr += n * h * w * c * sizeof(TS_FLOAT);
            break;
        }
        case RNE_NET_INPUT_TYPE_FLOAT32_CHW: {
            TS_FLOAT *src = (TS_FLOAT *)src_ptr;
            if (q_scale == -1) {
                TS_FLOAT *dst_float = (TS_FLOAT *)dst;
                TS_S32 idx = 0;
                for (TS_S32 bat = 0; bat < n; ++bat) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                dst_float[idx++] = src[(k * h + i) * w + j];
                            }
                        }
                    }
                    src += h * w * c;
                }
            } else {
                for (TS_S32 bat = 0; bat < n; bat++) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                TS_S64 data = 0;
                                TS_DOUBLE tmp = (TS_DOUBLE)src[(k * h + i) * w + j] * scale_int;
                                tmp = tmp > scale64_max ? scale64_max : tmp;
                                tmp = tmp < scale64_min ? scale64_min : tmp;
                                if (blob->s32BitNum == 8) {
                                    tmp /= (1 << q_scale);
                                    data = ((((TS_S64)(tmp * (1 << q_scale)) | (1LL << 62)) >> 62) *
                                                0.5f +
                                            tmp);
                                } else {
                                    data = ((TS_S64)(tmp)) >> q_scale;
                                }
                                data = data > data_max ? data_max : data;
                                data = data < data_min ? data_min : data;
                                for (TS_S32 u = 0; u < unit_size; ++u) {
                                    dst[k * unit_size + u] = data >> (u * CHAR_BIT);
                                }
                            }
                            dst += c_stride * unit_size;
                        }
                    }
                    dst = (TS_U8 *)(((size_t)dst + BLOB_ALIGN_BYTES - 1) &
                                    (~((size_t)BLOB_ALIGN_BYTES - 1)));
                    src += h * w * c;
                }
            }
            src_ptr += n * h * w * c * sizeof(TS_FLOAT);
            break;
        }
        case RNE_NET_INPUT_TYPE_INT8_HWC: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            for (TS_S32 bat = 0; bat < n; bat++) {
                for (TS_S32 i = 0; i < h * w; ++i) {
                    memcpy(dst, src, c * unit_size);
                    src += c * unit_size;
                    dst += c_stride * unit_size;
                }
                dst = (TS_U8 *)(((size_t)dst + BLOB_ALIGN_BYTES - 1) &
                                (~((size_t)BLOB_ALIGN_BYTES - 1)));
            }
            src_ptr += n * h * w * c * unit_size;
            break;
        }
        case RNE_NET_INPUT_TYPE_INT8_CHW: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            for (TS_S32 bat = 0; bat < n; bat++) {
                for (TS_S32 i = 0; i < h; ++i) {
                    for (TS_S32 j = 0; j < w; ++j) {
                        for (TS_S32 k = 0; k < c; ++k) {
                            for (TS_S32 u = 0; u < unit_size; ++u) {
                                TS_S32 src_idx = ((k * h + i) * w + j) * unit_size + u;
                                dst[k * unit_size + u] = src[src_idx];
                            }
                        }
                        dst += c_stride * unit_size;
                    }
                }
                dst = (TS_U8 *)(((size_t)dst + BLOB_ALIGN_BYTES - 1) &
                                (~((size_t)BLOB_ALIGN_BYTES - 1)));
                src += h * w * c * unit_size;
            }
            src_ptr += n * h * w * c * unit_size;
            break;
        }
        case RNE_NET_INPUT_TYPE_FORMAT_0RGB:
        case RNE_NET_INPUT_TYPE_FORMAT_GREY: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            memcpy(dst, src, n * h * w * c_stride * unit_size);
            src_ptr += n * h * w * c_stride * unit_size;
            break;
        }
        default:
            TS_MPI_TRP_RNE_Error("unsupport inputype:%d\n", net->eInputType);
            return -1;
            break;
        }
    }
    return 0;
}
