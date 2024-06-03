//
// Created by xiong on 2022/7/9.
//

#include "EnvZone.hpp"

using namespace Emx;

ErrCodeE EnvZone::SetOvdZone(const Json::Value &json) {
    if (json.type() != Json::arrayValue) {
        emxloge("%d it's not json array\n", (int) json.type());
        return ErrCodeE::ParseFailed;
    }
    memset(map, 0, sizeof(map));
    for (auto &area : json) {
        //解析每一个areaID数组，总分为25块，确定哪些区域块需要进行设置
        //这里只需解析areaId参数即能确认区域问题
        if (!area.isMember("areaId")) {
            emxloge("cannot found areaId\n");
            continue;
        }
        emxlogd("areaid is:%d\n", area["areaId"].asInt());

        if (!area.isMember("point")) {
            emxloge("cannot found point\n");
            continue;
        }
        //todo:不规则多边形处理
        int lx = INT32_MAX, ly = INT32_MAX, rx = 0, ry = 0;
        for (auto &point : area["point"]) {
            if (!point.isMember("pointId")) {
                emxloge("cannot found pointId\n");
                continue;
            }
            int pointId = point["pointId"].asInt();
            int px = point["x"].asInt();
            int py = point["y"].asInt();
            emxlogd("point%d:(%d,%d)\n", pointId, px, py);
            //选取左上角
            if (px <= lx && py <= ly) {
                lx = px;
                ly = py;
            }
            //选取右下角
            if (px >= rx && py >= ry) {
                rx = px;
                ry = py;
            }
        }
        lx = lx * Width / 10000;
        rx = rx * Width / 10000;
        ly = ly * Height / 10000;
        ry = ry * Height / 10000;
        emxlogd("final rect point[%d,%d]-[%d,%d]\n", lx, ly, rx, ry);
        for (int i = 0; i < Height; i++) {
            for (int j = 0; j < Width; j++) {
                if ((lx <= j) && (rx >= j)
                    && (ly <= i) && (ry >= i)) {
                    map[i * Width + j] = true;
                }
            }
        }
    }
    return ErrCodeE::Success;
}

ErrCodeE EnvZone::SetOvdZoneEx(const Json::Value &json) {
    if (json.type() != Json::arrayValue) {
        emxloge("%d it's not json array\n", (int) json.type());
        return ErrCodeE::ParseFailed;
    }
    memset(map, 0, sizeof(map));
    for (auto &area : json) {
        //解析每一个areaID数组，总分为25块，确定哪些区域块需要进行设置
        //这里只需解析areaId参数即能确认区域问题
        if (!area.isMember("areaId")) {
            emxloge("cannot found areaId\n");
            continue;
        }
        emxlogd("areaid is:%d\n", area["areaId"].asInt());

        if (!area.isMember("point")) {
            emxloge("cannot found point\n");
            continue;
        }
        PolygonFiller polygon(Width, Height);
        for (auto &point : area["point"]) {
            if (!point.isMember("pointId")) {
                emxloge("cannot found pointId\n");
                continue;
            }
            int pointId = point["pointId"].asInt();
            int px = point["x"].asInt();
            int py = point["y"].asInt();
            emxlogd("point%d:(%d,%d)\n", pointId, px, py);
            px = px * Width / 10000;
            py = py * Height / 10000;
            emxlogd("final rect point[%d,%d]\n", px, py);
            polygon.AddPoint(px, py);
        }

        //填充多边形区域
        polygon.FillPolygon();

        // 测试打印填充后的地图
        // printf("polygon: \n");
        // polygon.PrintMap();
        
        // 全量赋值给map
        for (int i = 0; i < Width * Height; ++i) {
            map[i] = polygon.m_map[i];
        }

        // 测试打印填充后的地图
        // printf("zone: \n");
        // for (int y = 0; y < Height; y++) {
        //     for (int x = 0; x < Width; x++) {
        //         printf("%d", (int)map[y * Width + x]);
        //     }
        //     printf("\n");
        // }
    }
    return ErrCodeE::Success;
}

