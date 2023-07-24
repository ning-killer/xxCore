//
// Created by xiong on 2022/4/21.
//

#ifndef EMX_PanoQrPreProcess_HPP
#define EMX_PanoQrPreProcess_HPP

namespace Emx {

    class PanoQrPreProcess {
    public:
        typedef unsigned char ep_uint8;
        typedef struct EpPoint_st {
            int x;
            int y;
        } EpPoint;

        typedef struct EpSize_st {
            int w;
            int h;
        } EpSize;

        typedef struct EpPoint2f_st {
            float x;
            float y;
        } EpPoint2f;

        typedef struct EpRect_st {
            int x;
            int y;
            int w;
            int h;
        } EpRect;


        int preProcessInitWithMap(EpSize yimg_size, EpPoint center_l, EpPoint center_r,
                                  short *y_mapX, short *y_mapY, int mirror_flag);

        int preProcess(ep_uint8 *dataY, ep_uint8 *dataU);

        ep_uint8 *GetRemapDataY(EpRect *rect);

        void FreeAllBuffer();

    private:
        int epFindRect(ep_uint8 *data, int w, int h, int size_y,
                       int centerY_x, int centerY_y, int threshold, EpRect *rect);

    private:
        struct EpQRcodeDetect {
            EpSize yimg_size;
            EpPoint center_y_l;
            EpPoint center_y_r;
            int mirror_flag;

            int left_or_right;//0 is left circle, 1 is right circle
            int size_y;
            float scale;
            EpRect qr_rect;

            int radius_y;

            ep_uint8 *remap_y_data;
            float *y_l_mapX;
            float *y_l_mapY;
            float *y_r_mapX;
            float *y_r_mapY;
            short *y_mapX;
            short *y_mapY;
        } m_detect;
    };

}

#endif //EMX_PanoQrPreProcess_HPP
