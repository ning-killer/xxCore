//
// Created by xiong on 2022/7/8.
//

#ifndef EMX_Zone_HPP
#define EMX_Zone_HPP

#include "EmxCore.hpp"

namespace Emx {
    //!<多边形区域填充器
    class PolygonFiller {
      public:
        struct Point {
            Point(int ix, int iy) : x(ix), y(iy) {}
            int x;
            int y;
        };

        PolygonFiller(int width, int height);
        void AddPoint(int x, int y);
        void FillPolygon();
        void PrintMap();

      private:
        bool IsInsidePolygon(int x, int y);

      private:
        int m_width;
        int m_height;
        std::vector<Point> m_points;
      public:
        std::vector<bool> m_map;
    };
    class Zone {
      public:
        static const int Width = 40;
        static const int Height = 30;

        Zone() : map() {}

        ErrCodeE Parse(const Json::Value &json);

        ErrCodeE Serialize(Json::Value &json);

        bool map[Width * Height];
    };

}

#endif //EMX_Zone_HPP
