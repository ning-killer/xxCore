//
// Created by xiong on 2022/7/8.
//

#include "Zone.hpp"
#include <queue>

using namespace Emx;

PolygonFiller::PolygonFiller(int width, int height)
    : m_width(width)
    , m_height(height) {
    m_map.resize(width * height, false);
}

void PolygonFiller::AddPoint(int x, int y) {
    m_points.emplace_back(x, y);
}

void PolygonFiller::FillPolygon() {
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            if (IsInsidePolygon(x, y)) {
                m_map[y * m_width + x] = 1;
            }
        }
    }
}

 bool PolygonFiller::IsInsidePolygon(int x, int y) {
    int numPoints = m_points.size();
    bool inside = false;
    for (int i = 0, j = numPoints - 1; i < numPoints; j = i++) {
        int xi = m_points[i].x;
        int yi = m_points[i].y;
        int xj = m_points[j].x;
        int yj = m_points[j].y;

        // 检查点 (x, y) 是否在边界线段 (xi, yi) 到 (xj, yj) 内部
        if (((yi > y) != (yj > y)) &&
            (x < (xj - xi) * (y - yi) / (double)(yj - yi) + xi)) {
            inside = !inside;
        }
    }
    return inside;
}

void PolygonFiller::PrintMap() {
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            printf("%d", (int)m_map[y * m_width + x]);
        }
        printf("\n");
    }
}

ErrCodeE Zone::Parse(const Json::Value &json) {
    if (json.size() != Width * Height / 8)
        return ErrCodeE::Failure;
    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width / 8; j++) {
            auto data = (uint8_t) json[i * Width / 8 + j].asInt();
            for (int k = 7; k >= 0; k--)
                map[i * Width + j * 8 + 7 - k] = (bool) ((data >> k) & 0x01);
        }
    return ErrCodeE::Success;
}

ErrCodeE Zone::Serialize(Json::Value &json) {
    json.clear();
    for (int i = 0; i < Height; i++)
        for (int j = 0; j < Width / 8; j++) {
            uint8_t data = 0;
            for (int k = 7; k >= 0; k--)
                data |= ((uint8_t) map[i * Width + j * 8 + 7 - k]) << k;
            json.append(data);
        }
    return ErrCodeE::Success;
}
