//
// Created by xiong on 2022/4/26.
//
#include "media/MediaRegion.hpp"

using namespace Emx;

Rect Rect::Transfer(Size &srcSize, Size &dstSize) {
    Rect dst;
    dst.x = x * dstSize.w / srcSize.w;
    dst.x = dst.x < 0 ? 0 : dst.x;
    dst.x = dst.x >= dstSize.w ? dstSize.w - 1 : dst.x;

    dst.y = y * dstSize.h / srcSize.h;
    dst.y = dst.y < 0 ? 0 : dst.y;
    dst.y = dst.y >= dstSize.h ? dstSize.h - 1 : dst.y;

    dst.w = w * dstSize.w / srcSize.w;
    dst.w = dst.x + dst.w > dstSize.w ? dstSize.w - dst.x : dst.w;

    dst.h = h * dstSize.h / srcSize.h;
    dst.h = dst.y + dst.h > dstSize.h ? dstSize.h - dst.y : dst.h;
    return dst;
}
//
//bool RegionMask::HitMap(Size &srcSize, Rect &rect, RegionMask::Map &map) {
//    Size dstSize(RegionMask::Width, RegionMask::Height);
//    Rect dstRect = rect.Transfer(srcSize, dstSize);
//    for (int y = dstRect.y; y < dstRect.y + dstRect.h; y++)
//        for (int x = dstRect.x; x < dstRect.x + dstRect.w; x++)
//            if (map.block[dstSize.w * y + x])
//                return true;
//    return false;
//}
