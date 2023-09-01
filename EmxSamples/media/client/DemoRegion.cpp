/**
 * @file   DemoRegion.cpp
 * @author hening
 * @brief  媒体服务Region业务单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    // 原始框
    Rect srcRect(50,50,50,50);
    // 原始画幅大小
    Size srcSize(1920, 1080);
    // 目标画幅大小
    Size dstSize(1000, 800);

    // 转化目标框
    Rect dstRect = srcRect.Transfer(srcSize, dstSize);

    emxlogd("src Rect x[%d];y[%d];w[%d];h[%d] in SizeImg[%d, %d].\n"
            , srcRect.x, srcRect.y, srcRect.w, srcRect.h, srcSize.w, srcSize.h);
    
    emxlogd("dst Rect x[%d];y[%d];w[%d];h[%d] in SizeImg[%d, %d].\n"
            , dstRect.x, dstRect.y, dstRect.w, dstRect.h, dstSize.w, dstSize.h);
    return 0;
}