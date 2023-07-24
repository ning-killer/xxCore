//
// Created by cx on 2023/6/26.
//
#include <cstdlib>
#include <cstring>
#include "core/EmxLog.hpp"
#include "Raw2Jpge.hpp"
#include "turbojpeg.h"


using namespace Emx;

int Raw2Jpeg::raw_encode_jpeg(Raw2JpegInfo *prawInfo, unsigned char **jpegBuffer, unsigned long *jpegSize) {

    tjhandle handle = NULL;
    int flags = 0;
    int padding = 1;
    unsigned long need_size = 0;
    int ret = 0;

    handle = tjInitCompress();
    if (handle == NULL) {
        emxloge("jpeg create failed with!\n");
        return -1;
    }

    flags |= 0;
    if (prawInfo->pixel_format >= RAW2JPEG_MPDE_RGB) {
        prawInfo->pixel_format -= RAW2JPEG_MPDE_RGB;
        ret = tjCompress2(handle, prawInfo->src_buffer, prawInfo->width, prawInfo->stride, prawInfo->height,
                          prawInfo->pixel_format, jpegBuffer, jpegSize, TJSAMP_422, prawInfo->jpeg_quality, flags);
        if (ret < 0) {
            emxloge("tjCompress2 to jpeg failed: %s, praw_info->width:%u, praw_info->stride:%u, praw_info->height:%u\n",
                    tjGetErrorStr(), prawInfo->width, prawInfo->stride, prawInfo->height);
        }
    } else {
        need_size = tjBufSizeYUV2(prawInfo->width, padding, prawInfo->height, prawInfo->pixel_format);
        if (need_size != prawInfo->src_size) {
            emxloge("we detect yuv size: %d, but you give: %d, check again(%dx%d).\n", need_size, prawInfo->src_size,
                    prawInfo->width, prawInfo->height);
            return -1;
        }
        ret = tjCompressFromYUV(handle, prawInfo->src_buffer, prawInfo->width, padding, prawInfo->height,
                                prawInfo->pixel_format, jpegBuffer, jpegSize, prawInfo->jpeg_quality, flags);
        if (ret < 0) {
            emxloge("tjBufSizeYUV2 to jpeg failed: %s\n", tjGetErrorStr());
        }
    }
    tjDestroy(handle);
    return 0;
}

void Raw2Jpeg::Yuv420spToYuv420p(int mode, unsigned char *yuv420sp, int width, int height) {
    int i, j;
    int y_size = width * height;
    int uv_size = y_size / 2;
    unsigned char *tmp_uv = (unsigned char *) malloc(uv_size);
    if (!tmp_uv) {
        emxloge("Yuv420spToYuv420p malloc fail!\n");
        return;
    }

    unsigned char *tmp_420p = yuv420sp + y_size;
    unsigned char *uv = yuv420sp + y_size;

    unsigned char *u_tmp = tmp_uv;
    unsigned char *v_tmp = tmp_uv + uv_size / 2;
    if (RAW2JPEG_MODE_NV12 == mode) {
        u_tmp = tmp_uv + uv_size / 2;
        v_tmp = tmp_uv;
    }
    // uv
    for (j = 0, i = 0; j < y_size / 2; j += 2, i++) {
        v_tmp[i] = uv[j];
        u_tmp[i] = uv[j + 1];
    }
    memcpy(tmp_420p, tmp_uv, uv_size);
    if (tmp_uv)
        free(tmp_uv);
}

int Raw2Jpeg::RawToJpeg(Raw2JpegInfo *prawInfo, unsigned char **jpegBuffer, unsigned long *jpegSize) {
    if ((prawInfo->pixel_format == RAW2JPEG_MODE_NV12)
        || (prawInfo->pixel_format == RAW2JPEG_MODE_NV21)) {
        prawInfo->pixel_format = RAW2JPEG_MODE_420P;
        //要先将yuv420sp转换为yuv420p
        Yuv420spToYuv420p(prawInfo->pixel_format, prawInfo->src_buffer, prawInfo->width, prawInfo->height);
    }
    return raw_encode_jpeg(prawInfo, jpegBuffer, jpegSize);
}