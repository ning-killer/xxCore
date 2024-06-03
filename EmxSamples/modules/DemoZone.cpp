/**
 * @file   DemoZone.cpp
 * @author hening
 * @brief  区域业务测试
 * @date   2023-09-11 补充多边形区域填充测试
 */

#include "EmxCore.hpp"
#include "Zone.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    PolygonFiller filler(Zone::Width, Zone::Height);
    printf("src zone: \n");
    filler.PrintMap();
    filler.AddPoint(0, 0);
    filler.AddPoint(40, 0);
    filler.AddPoint(20, 30);
    filler.AddPoint(0, 15);
    filler.FillPolygon();
    printf("dst zone: \n");
    filler.PrintMap();
    return 0;
}