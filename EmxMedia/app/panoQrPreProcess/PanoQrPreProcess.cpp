//
// Created by xiong on 2022/4/21.
//
//-----------------------------------------------------------------------
//
// FILE       NAME : epPreQRGray.cpp
//
// CREATED    BY   : weixing at 05/23/2017, 14:10
//
// FUNCTION        :���û�ɫ���ûҶ�ͼ��������ά��Ԥʶ��
//-----------------------------------------------------------------------
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include "PanoQrPreProcess.hpp"

using namespace Emx;

#define PI 3.1415926

//static void GnerateMap(int size, int r, short *mapX, short *mapY) {
//    //cv::Mat imgx, imgy;
//    //imgx.create(cv::Size(size, size), CV_32FC1);
//    //imgy.create(cv::Size(size, size), CV_32FC1);
//    //imgx.data = (unsigned char *)mapX;
//    //imgy.data = (unsigned char *)mapY;
//    int i, j;
//    float half_size;
//    double row, col;
//    float distance, theta, radiu, alpha;
//
//    half_size = (float) size / 2;
//    for (i = 0; i < size; i++) {
//        row = i - half_size;
//        for (j = 0; j < size; j++) {
//            col = j - half_size;
//
//            distance = sqrt(row * row + col * col) * 1.5;//1.3¿ØÖÆÐ£ÕýºóµÄÍ¼Ïñ´óÐ¡£¬µ÷´óÍ¼Ïñ¾ÍÐ¡£¬µ÷Ð¡Í¼Ïñ¾Í´ó 400W
//            //distance = sqrt(row*row + col*col) * 1.3;//1.3¿ØÖÆÐ£ÕýºóµÄÍ¼Ïñ´óÐ¡£¬µ÷´óÍ¼Ïñ¾ÍÐ¡£¬µ÷Ð¡Í¼Ïñ¾Í´ó
//
//            theta = distance / r;
//            radiu = 2.7 * r * sin(theta / 2); //400W
//            //radiu = 2.0 * r * sin(theta / 2);
//            alpha = atan2(row, col + 0.000000001);
//
//            mapX[i * size + j] = (short) (radiu * cos(alpha));
//            mapY[i * size + j] = (short) (radiu * sin(alpha));
//        }
//    }
//
//#if 0
//    FILE *fp_mapX = fopen("Y_mapX_200w.bin", "wb");
//    fwrite(mapX, sizeof(short), size * size, fp_mapX);
//    fclose(fp_mapX);
//    FILE *fp_mapY = fopen("Y_mapY_200w.bin", "wb");
//    fwrite(mapY, sizeof(short), size * size, fp_mapY);
//    fclose(fp_mapY);
//#endif
//
//}


int PanoQrPreProcess::preProcessInitWithMap(PanoQrPreProcess::EpSize yimg_size,
                                            PanoQrPreProcess::EpPoint center_l,
                                            PanoQrPreProcess::EpPoint center_r,
                                            short *y_mapX, short *y_mapY, int mirror_flag) {
    float Radiu_y;
    int height_y = yimg_size.h;
    int size_y;
    //short* y_mapY;
    //short* y_mapX;

    if (height_y == 1520 || height_y == 1944 || height_y == 1440) {
        Radiu_y = 130;
    }
    if (height_y == 1080) {
        Radiu_y = 130;
    }

    size_y = (int) (Radiu_y * PI);
    //y_mapX = (short*)malloc(size_y*size_y*sizeof(short));
    //y_mapY = (short*)malloc(size_y*size_y*sizeof(short));
    //GnerateMap(size_y, Radiu_y, y_mapX, y_mapY);

    m_detect.yimg_size.w = yimg_size.w;
    m_detect.yimg_size.h = yimg_size.h;
    m_detect.center_y_l.x = center_l.x;
    m_detect.center_y_l.y = center_l.y;
    m_detect.center_y_r.x = center_r.x;
    m_detect.center_y_r.y = center_r.y;
    m_detect.left_or_right = 0;
    m_detect.size_y = size_y;
    m_detect.radius_y = Radiu_y;
    m_detect.remap_y_data = (unsigned char *) malloc(size_y * size_y * sizeof(unsigned char));
    m_detect.y_mapX = y_mapX;
    m_detect.y_mapY = y_mapY;
    return 1;
}

//static int GetThreshold(PanoQrPreProcess::ep_uint8 *dataY, int w, int h, int centerY_x, int centerY_y) {
//    int i, j;
//    int threshold = 0;
//    int range = h / 10;
//    for (i = (centerY_y - range); i < (centerY_y + range); i++) {
//        for (j = (centerY_x - range); j < (centerY_x + range); j++) {
//            threshold += dataY[i * w + j];
//        }
//    }
//    threshold = threshold / (range * range * 4);
//    return threshold;
//}

//static int UndistoredTransform(int center_x, int center_y, int r, int half_size, PanoQrPreProcess::EpPoint src,
//                               PanoQrPreProcess::EpPoint *dst) {
//    float src_r, dst_r, theta, alpha, beta;
////    float radiu;
//    double tan_x, tan_y;
//    tan_x = src.x - center_x;
//    tan_y = src.y - center_y;
//    src_r = sqrt(tan_x * tan_x + tan_y * tan_y);//1.3����У�����ͼ���С������ͼ���С����Сͼ��ʹ�
//    alpha = atan2(tan_y, tan_x + 0.000000001);
//    beta = src_r / (2 * r);
//    if (beta > 1) {
//        dst->x = 0;
//        dst->y = 0;
//        return 0;
//    }
//    theta = 2 * asin(beta);
//    dst_r = theta * r / 1.5;
//    dst->x = dst_r * cos(alpha) + half_size;
//    dst->y = dst_r * sin(alpha) + half_size;
//
//    return 1;
//}

static int
epFindRectChild(PanoQrPreProcess::ep_uint8 *data, int w, int h, int searchDistance, int center_x, int center_y,
                int threshold) {
//    printf("w: %d\n", w);
//    printf("h: %d\n", h);
//    printf("searchDistance: %d\n", searchDistance);
//    printf("center_x: %d\n", center_x);
//    printf("center_y: %d\n", center_y);
//    printf("threshold: %d\n", threshold);
    int sum[4] = {0};
    int temp;
    int i;
    PanoQrPreProcess::ep_uint8 value = 0;
    value = data[center_y * w + center_x] > threshold ? 255 : 0;
    for (i = 0; i < searchDistance; i++) {
        temp = data[center_y * w + center_x - i] > threshold ? 255 : 0;
        if (temp != value) {
            i = i + 2;
            sum[0] = sum[0] + 1;
        }
    }
    value = data[center_y * w + center_x] > threshold ? 255 : 0;
    for (i = 0; i < searchDistance; i++) {
        temp = data[center_y * w + center_x + i] > threshold ? 255 : 0;
        if (temp != value) {
            i = i + 2;
            sum[1] = sum[1] + 1;
        }
    }
    value = data[center_y * w + center_x] > threshold ? 255 : 0;
    for (i = 0; i < searchDistance; i++) {
        temp = data[(center_y - i) * w + center_x] > threshold ? 255 : 0;
        if (temp != value) {
            i = i + 2;
            sum[2] = sum[2] + 1;
        }
    }
    value = data[center_y * w + center_x] > threshold ? 255 : 0;
    for (i = 0; i < searchDistance; i++) {
        temp = data[(center_y + i) * w + center_x] > threshold ? 255 : 0;
        if (temp != value) {
            i = i + 2;
            sum[3] = sum[3] + 1;
        }
    }

    // cout << "sum[0]:" << sum[0] << ", sum[1]:" << sum[1] << ", sum[2]:" << sum[2] << ", sum[3]:" << sum[3] << endl;
    int singleSideT = 6;
    int allSideT = 40;
    if (sum[0] > singleSideT &&
        sum[1] > singleSideT &&
        sum[2] > singleSideT &&
        sum[3] > singleSideT &&
        (sum[0] + sum[1] + sum[2] + sum[3]) > allSideT) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return 1;
    } else {
        // printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return 0;
    }
}

int PanoQrPreProcess::epFindRect(PanoQrPreProcess::ep_uint8 *data,
                                 int w, int h, int size_y, int centerY_x, int centerY_y, int threshold,
                                 PanoQrPreProcess::EpRect *rect) {
    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
    //cv::Mat imgY;
    //imgY.create(cv::Size(m_detect.yimg_size.w, m_detect.yimg_size.h), CV_8UC1);
    //imgY.data = data; //cv::namedWindow("imgY", 0);
    int i;
    int result = 0;
    int searchDistance = size_y / 3;// h / 4;
//    int half_size = h / 2;
    int center_x = centerY_x;
    int center_y = centerY_y;
//    PanoQrPreProcess::EpPoint up = {0}, down = {0}, left = {0}, right = {0};
//    float radius_y = m_detect.radius_y;
    int expandDis;
    int findRectCount;
    //int size_y = m_detect.size_y;
    //int threshold = 0;

    //threshold = 128;

    if (h == 1520 || h == 1944 || h == 1440) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        findRectCount = 10;
        searchDistance = 225;
    } else if (h == 1080) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        findRectCount = 5;
        searchDistance = 182;
    } else {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return -1;
    }
    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
    result = epFindRectChild(data, w, h, searchDistance, center_x, center_y, threshold);
    if (result == 0) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return 0;
    }
    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
    expandDis = 4;
    for (i = 1; i < findRectCount; i++) {
        result = epFindRectChild(data, w, h, searchDistance, center_x - expandDis * i, center_y - expandDis * i,
                                 threshold);
        if (result == 0) {
            printf("[%s:%d]\n", __FUNCTION__, __LINE__);
            return 0;
        }
        result = epFindRectChild(data, w, h, searchDistance, center_x - expandDis * i, center_y + expandDis * i,
                                 threshold);
        if (result == 0) {
            printf("[%s:%d]\n", __FUNCTION__, __LINE__);
            return 0;
        }
        result = epFindRectChild(data, w, h, searchDistance, center_x + expandDis * i, center_y - expandDis * i,
                                 threshold);
        if (result == 0) {
            printf("[%s:%d]\n", __FUNCTION__, __LINE__);
            return 0;
        }
        result = epFindRectChild(data, w, h, searchDistance, center_x + expandDis * i, center_y + expandDis * i,
                                 threshold);
        if (result == 0) {
            printf("[%s:%d]\n", __FUNCTION__, __LINE__);
            return 0;
        }
    }
    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
    rect->x = center_x - searchDistance;
    rect->y = center_y - searchDistance;
    rect->w = searchDistance * 2;
    rect->h = searchDistance * 2;
    return 1;
    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
}

static void remap_withroi_withmirror_nearest(unsigned char *srcImg, unsigned char *dstImg,
                                             short *mapX, short *mapY, int centerY_x, int centerY_y,
                                             int inWidth, int inHeight, int size,
                                             PanoQrPreProcess::EpRect out_rect, int flag) {
    int i, j;
    short outStartX = out_rect.x;
    short outStartY = out_rect.y;
    short outWidth = out_rect.w;
    short outHeight = out_rect.h;
    //
    for (i = 0; i < outHeight; i++) {
        for (j = 0; j < outWidth; j++) {
            short xIndex = (short) (mapX[(i + outStartY) * (size) + (j + outStartX)] + centerY_x);
            short yIndex = (short) (mapY[(i + outStartY) * (size) + (j + outStartX)] + centerY_y);

            dstImg[i * (outWidth) + (outWidth - 1 - j)] = (unsigned char) srcImg[yIndex * inWidth + xIndex];
        }
    }
}

int PanoQrPreProcess::preProcess(PanoQrPreProcess::ep_uint8 *dataY, PanoQrPreProcess::ep_uint8 *dataU) {
    //int64 tt;
    //tt = cv::getTickCount();
    //cv::Mat imgY;
    //imgY.create(cv::Size(m_detect.yimg_size.w, m_detect.yimg_size.h), CV_8UC1);
    //imgY.data = dataY; //cv::namedWindow("imgY", 0);
    //cv::Mat img_yl;
    //img_yl.create(cv::Size(m_detect.size_y, m_detect.size_y), CV_8UC1);
    //img_yl.data = m_detect.remap_y_data;
    //cv::Mat mapX, mapY;
    //mapX.create(cv::Size(m_detect.size_y, m_detect.size_y), CV_32FC1);
    //mapY.create(cv::Size(m_detect.size_y, m_detect.size_y), CV_32FC1);
    //mapX.data = (unsigned char *)m_detect.y_mapX;
    //mapY.data = (unsigned char *)m_detect.y_mapY;

    int result = 1;
    int Yw, Yh;
    int centerY_x, centerY_y;
    int threshold = 72;
    PanoQrPreProcess::EpRect rect;
    PanoQrPreProcess::ep_uint8 *remap_y_data = m_detect.remap_y_data;
    short *y_mapX, *y_mapY;
    int size_y = m_detect.size_y;
//    int expand = 0;

    Yw = m_detect.yimg_size.w;
    Yh = m_detect.yimg_size.h;
    if (m_detect.left_or_right == 0) {
        centerY_x = m_detect.center_y_l.x;
        centerY_y = m_detect.center_y_l.y;
    } else {
        centerY_x = m_detect.center_y_r.x;
        centerY_y = m_detect.center_y_r.y;
    }
    y_mapX = m_detect.y_mapX;
    y_mapY = m_detect.y_mapY;
    m_detect.left_or_right = 1 - m_detect.left_or_right;


    result = epFindRect(dataY, Yw, Yh, size_y, centerY_x, centerY_y, threshold, &rect);
    //cout << "  -------------------------preProcess time is : " << (cv::getTickCount() - tt) / cv::getTickFrequency() * 1000 << endl;
    if (result == 0) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return 0;
    }
    //m_detect.qr_rect.x = ((rect.x - expand) < 0) ? 0 : (rect.x - expand);
    //m_detect.qr_rect.y = ((rect.y - expand) < 0) ? 0 : (rect.y - expand);
    //m_detect.qr_rect.w = ((rect.w + m_detect.qr_rect.x + expand*2) >= size_y) ? (size_y - 1 - m_detect.qr_rect.x) : (rect.w + expand*2);
    //m_detect.qr_rect.h = ((rect.h + m_detect.qr_rect.y + expand*2) >= size_y) ? (size_y - 1 - m_detect.qr_rect.y) : (rect.h + expand*2);
    //m_detect.qr_rect.x = rect.x;
    //m_detect.qr_rect.y = rect.y;
    m_detect.qr_rect.w = size_y;
    m_detect.qr_rect.h = size_y;
    if (m_detect.qr_rect.x < 0 ||
        m_detect.qr_rect.y < 0 ||
        m_detect.qr_rect.w <= 0 ||
        m_detect.qr_rect.h <= 0) {
        printf("[%s:%d]\n", __FUNCTION__, __LINE__);
        return 0;
    }
    remap_withroi_withmirror_nearest(dataY, remap_y_data, y_mapX, y_mapY, centerY_x, centerY_y, Yw, Yh, size_y,
                                     m_detect.qr_rect, 1);
    return 1;
}

unsigned char *PanoQrPreProcess::GetRemapDataY(PanoQrPreProcess::EpRect *rect) {
    rect->x = m_detect.qr_rect.x;
    rect->y = m_detect.qr_rect.y;
    rect->w = m_detect.qr_rect.w;
    rect->h = m_detect.qr_rect.h;
    return m_detect.remap_y_data;
}

void PanoQrPreProcess::FreeAllBuffer() {
    free(m_detect.remap_y_data);
}
