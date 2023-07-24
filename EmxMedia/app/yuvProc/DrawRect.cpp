//
// Created by SimonXiao on 2022/3/27.
//

#include <cstring>
#include "DrawRect.hpp"

using namespace Emx;

DrawRect::Color DrawRect::nv21[(int) DrawRect::ColorE::Max] = {
        {0x92, 0x8F, 0x17},//杭研绿色
        {0x4c, 0x54, 0xFF},//红色
        {0x1d, 0xFF, 0x6B},//蓝色
        {0xFF, 0x80, 0x80},//白色
        {0x00, 0x80, 0x80},//黑色
};

DrawRect::Color DrawRect::nv12[(int) DrawRect::ColorE::Max] = {
        {0x92, 0x17, 0x8F},//杭研绿色
        {0x4c, 0xFF, 0x54},//红色
        {0x1d, 0x6B, 0xFF},//蓝色
        {0xFF, 0x80, 0x80},//白色
        {0x00, 0x80, 0x80},//黑色
};

void DrawRect::Nvxx(DrawRect::Color *color, uint8_t *y, uint8_t *uv, Rect &rect, int stride, int lineWidth) {
    /*先处理y数据*/
    for (int i = rect.y; i < rect.h; i++) {
        /*step 为x轴的起始方向*/
        int step = i * stride + rect.x;
        /*矩形框的上下边，y轴在开始和结束时画出线宽的线*/
        if (((i - rect.y) < lineWidth) || ((rect.h - i) <= lineWidth)) {
            memset(y + step, color->y, rect.w);
        } else {
            /*矩形框的左右边，x轴在开始画出线宽的线*/
            memset(y + step, color->y, lineWidth);
            memset(y + (step + rect.w - lineWidth), color->y, lineWidth);
        }
    }

    /*第二步 处理vu数据*/
    int vu_start_y = rect.y / 2;
    int end_vu_height = rect.h / 2;
    int vu_start_x = rect.x;
    int vu_data_width = rect.w;

    int vu_line_width = lineWidth / 2;
    if (vu_line_width < 1)vu_line_width = 1;
    for (int i = vu_start_y; i < end_vu_height; i++) {
        int step = i * stride + vu_start_x;    //定位到每一行的x开始位置
        /*矩形框的上下边，y轴在开始和结束时画出线宽的线*/
        if (((i - vu_start_y) < vu_line_width) || ((end_vu_height - i) <= vu_line_width)) {
            uint8_t *tmp_uv = uv + step;
            for (int j = 0; j < vu_data_width;) {
                tmp_uv[j] = color->c0;
                tmp_uv[j + 1] = color->c1;
                j += 2;
            }
        } else {
            uint8_t *tmp_vu_left = uv + step;
            for (int j = 0; j < lineWidth;) {
                tmp_vu_left[j] = color->c0;
                tmp_vu_left[j + 1] = color->c1;
                j += 2;
            }
            uint8_t *tmp_vu_right = tmp_vu_left + (vu_data_width - (lineWidth + 1) / 2 * 2);
            for (int j = 0; j < lineWidth;) {
                tmp_vu_right[j] = color->c0;
                tmp_vu_right[j + 1] = color->c1;
                j += 2;
            }
        }
    }
}
