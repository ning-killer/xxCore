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

uint32_t RgbaToArgb1555(uint32_t rgba) {
    uint8_t r = (rgba >> 24) & 0xFF;
    uint8_t g = (rgba >> 16) & 0xFF;
    uint8_t b = (rgba >> 8) & 0xFF;
    uint8_t a = (rgba) & 0xFF;

    // 将各个通道的8位值映射到5位
    uint16_t ar = (r >> 3) & 0x1F;
    uint16_t ag = (g >> 3) & 0x1F;
    uint16_t ab = (b >> 3) & 0x1F;
    uint16_t aa = (a >= 128) ? 0x1 : 0; // 如果透明度大于等于128，设置ARGB1555的透明位

    // 将各通道值合并为ARGB1555值
    uint16_t argb1555 = (aa << 15) | (ar << 10) | (ag << 5) | ab;

    return argb1555;
}

// void DrawLines(int num) {
//     std::vector<MediaRegion::Line> lines;
//     float s_x = 0.1;
//     float s_y = 0.9;
//     float e_x = 0.9;
//     float e_y = 0.1;
//     for (int i = 0; i < num; ++i) {
//         MediaRegion::Line line = { 0 };
//         line.color = RgbaToArgb1555(0x0000ffff);
//         line.thickness = 3;
//         line.sp.x = s_x;
//         line.sp.y = s_y;
//         line.ep.x = e_x;
//         line.ep.y = e_y;
//         e_y += 0.05;
//         lines.emplace_back(line);
//     }
//     for (auto line : lines) {
//         emxlogd("[(%.2f,%.2f),(%.2f,%.2f)->color[%x]]\n"
//             , line.sp.x, line.sp.y
//             , line.ep.x, line.ep.y
//             , line.color);
//     }
//     int ai_chn = 0;
//     MediaClientRegion region(ai_chn);
//     if (region.DrawLines(lines) == ErrCodeE::Success) {
//         emxlogd("DrawLines ok\n");
//         return;
//     }
//     emxloge("DrawLines failed\n");
// }

// void RunMediaClientRegion(Json::Value data) {
//     int ai_chn = 0;
//     for (int i = 0; i < (int)data.size(); ++i) {
//         ai_chn = i;
//         std::vector<MediaRegion::Line> lines;
//         for (auto obj : data[i]["line"]) {
//             MediaRegion::Line line = { 0 };
//             line.color = obj["color"].asUInt64();
//             line.thickness = obj["thickness"].asInt();
//             line.sp.x = obj["sp_x"].asFloat();
//             line.sp.y = obj["sp_y"].asFloat();
//             line.ep.x = obj["ep_x"].asFloat();
//             line.ep.y = obj["ep_y"].asFloat();
//             lines.emplace_back(line);
//         }
//         MediaClientRegion region(ai_chn);
//         if (region.DrawLines(lines) == ErrCodeE::Success) {
//             emxlogd("chn %d DrawLines ok\n", ai_chn);
//         }
//     }
// }

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

    // if (argc < 2) {
    //     emxloge("argument error: [线段数量]\n");
    //     return -1;
    // }
    // int num = std::stoi(argv[1]);
    // DrawLines(num);
    // if (argc < 2) {
    //     emxloge("argument error: [DemoRegion textFile]\n");
    //     return -1;
    // }
    // std::string textfile = std::string(argv[1]);
    
    // Json::Value dataJson;
    // if (EasyJson::Load(textfile.c_str(), dataJson) == ErrCodeE::Failure) {
    //     emxloge("read %s failed\n", textfile.c_str());
    //     return -1;
    // }
    // RunMediaClientRegion(dataJson);
    return 0;
}