//
// Created by cx on 2023/6/26.
//

#ifndef EMX_RAW2JPGE_H
#define EMX_RAW2JPGE_H

namespace Emx {

    typedef struct {
        unsigned char *src_buffer;
        unsigned int src_size;
        unsigned int width;
        unsigned int height;
        int pixel_format;
        int jpeg_quality;
        int stride;
    } Raw2JpegInfo;


    typedef enum
    {
        RAW2JPEG_MODE_444=0,
        RAW2JPEG_MODE_422P,
        RAW2JPEG_MODE_420P,
        RAW2JPEG_MODE_GRAY,
        RAW2JPEG_MODE_440,
        RAW2JPEG_MODE_411,
        RAW2JPEG_MODE_NV12,
        RAW2JPEG_MODE_NV21,

        RAW2JPEG_MPDE_RGB = 20,
        RAW2JPEG_MPDE_BGR,
        RAW2JPEG_MPDE_RGBX,
        RAW2JPEG_MPDE_BGRX,
        RAW2JPEG_MPDE_XBGR,
        RAW2JPEG_MPDE_XRGB,
        RAW2JPEG_MPDE_GRAY,
        RAW2JPEG_MPDE_RGBA,
        RAW2JPEG_MPDE_BGRA,
        RAW2JPEG_MPDE_ABGR,
        RAW2JPEG_MPDE_ARGB,
        RAW2JPEG_MPDE_CMYK
    }enRAW2JPEG_MODE;

    class Raw2Jpeg {
    public:
        static int RawToJpeg(Raw2JpegInfo *prawInfo, unsigned char **jpegBuffer, unsigned long *jpegSize);
    private:
        static void Yuv420spToYuv420p(int mode, unsigned char *yuv420sp, int width, int height);
        static int raw_encode_jpeg(Raw2JpegInfo *prawInfo,unsigned char** jpegBuffer, unsigned long * jpegSize);
    };
}


#endif //EMX_RAW2JPGE_H
