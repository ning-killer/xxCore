#ifndef EAPIL_EnvAlarmPassengerFlow_HPP
#define EAPIL_EnvAlarmPassengerFlow_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"
#include "EmxMedia.hpp"

namespace Emx {
    class EnvAlarmPassengerFlow : public EnvBase {
    public:
    //!@brief 抓拍模式
    enum SnapMode : uint8_t {
        Quality = 0, //!< 质量优先
        Quick,       //!< 速度优先
    }; 
    //!@brief 方向
    enum Direction : uint32_t {
        BottomToTop = 0,            //!< 下到上 
        TopToBottom = 1,            //!< 上到下 
    };
    //!@brief 方向
    enum DirectionRule : uint32_t {
        BottomIn_TopOut = 0,         //!< 上出下进 
        TopIn_BottomOut = 1,         //!< 上进下出
    };
    //!@brief 坐标点
    struct Point {
        int x = 0; //!< x轴坐标
        int y = 0; //!< y轴坐标
    };
    class StatisticsLine {
        public:
            StatisticsLine():m_ax(0), m_ay(0), m_bx(0), m_by(0) {}

            void Update(int A_x, int A_y, int B_x, int B_y) {
                m_ax = A_x; m_ay = A_y; m_bx = B_x, m_by = B_y;
            }

            void Parse(const Json::Value value) {
                m_ax = value["A_x"].asInt();
                m_ay = value["A_y"].asInt();
                m_bx = value["B_x"].asInt();
                m_by = value["B_y"].asInt();
            }

            void Serialize(Json::Value &value) {
                value["A_x"] = m_ax;
                value["A_y"] = m_ay;
                value["B_x"] = m_bx;
                value["B_y"] = m_by;
            }

            // 判断两个线段是否相交
            bool areSegmentsIntersecting(
                const Point& p1, const Point& q1, 
                const Point& p2, const Point& q2) {
                int o1 = orientation(p1, q1, p2);
                int o2 = orientation(p1, q1, q2);
                int o3 = orientation(p2, q2, p1);
                int o4 = orientation(p2, q2, q1);
                return (o1 != o2 && o3 != o4) ||
                    (o1 == 0 && onSegment(p1, p2, q1)) ||
                    (o2 == 0 && onSegment(p1, q2, q1)) ||
                    (o3 == 0 && onSegment(p2, p1, q2)) ||
                    (o4 == 0 && onSegment(p2, q1, q2));
            }

            // 判断点r是否在线段pq上
            bool onSegment(const Point& p, const Point& q, const Point& r) {
                return r.x <= std::max(p.x, q.x) && r.x >= std::min(p.x, q.x) &&
                    r.y <= std::max(p.y, q.y) && r.y >= std::min(p.y, q.y);
            }

            // 计算三点的方向，用于跨立判断
            int orientation(const Point& p, const Point& q, const Point& r) {
                double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
                return (val == 0) ? 0 : (val > 0) ? 1 : 2;
            }

            // 是否拌线：目标框和线段相交
            bool IsCross(const Rect &current, const Size &srcSize) {
                emxlogt("srcSize(%d,%d)\n", srcSize.w, srcSize.h);
                emxlogt("a(%d,%d); b(%d,%d)\n", m_ax, m_ay, m_bx, m_by);
                Point left_top_p = { current.x, current.y };
                Point left_bottom_p = { current.x, current.y + current.h };
                Point right_top_p = { current.x + current.w, current.y };
                Point right_bottom_p = { current.x + current.w, current.y +current.h };
                Point src_sp = { (int)(((float)m_ax / 10000) * srcSize.w),  (int)(((float)m_ay / 10000) * srcSize.h) };
                Point src_ep = { (int)(((float)m_bx / 10000) * srcSize.w),  (int)(((float)m_by / 10000) * srcSize.h) };
                emxlogt("left_top_p[%d,%d]\n", left_top_p.x, left_top_p.y);
                emxlogt("left_bottom_p[%d,%d]\n", left_bottom_p.x, left_bottom_p.y);
                emxlogt("right_top_p[%d,%d]\n", right_top_p.x, right_top_p.y);
                emxlogt("right_bottom_p[%d,%d]\n", right_bottom_p.x, right_bottom_p.y);
                emxlogt("src_sp[%d,%d];src_ep[%d,%d]\n", src_sp.x, src_sp.y, src_ep.x, src_ep.y);
                return  areSegmentsIntersecting(src_sp, src_ep, left_top_p, right_top_p) ||
                        areSegmentsIntersecting(src_sp, src_ep, right_top_p, right_bottom_p) ||
                        areSegmentsIntersecting(src_sp, src_ep, right_bottom_p, left_bottom_p) ||
                        areSegmentsIntersecting(src_sp, src_ep, left_bottom_p, left_top_p);
            }

            void CheckCrossDirection(Direction &direction, Rect previous, Rect current, const Size &srcSize) {
                direction = BottomToTop;
                //note: 判断当前目标框和前一个框的移动方向(只判断Direction方向，所以只对比前后2个目标框的中心点)
                Point previous_center_p = { previous.x + previous.w / 2, previous.y + previous.h / 2 };
                Point current_center_p = { current.x + current.w / 2, current.y + current.h / 2 };
                Point src_a = { (int)(((float)m_ax / 10000) * srcSize.w),  (int)(((float)m_ay / 10000) * srcSize.h) };
                Point src_b = { (int)(((float)m_bx / 10000) * srcSize.w),  (int)(((float)m_by / 10000) * srcSize.h) };
                emxlogt("src_a[%d,%d];src_b[%d,%d]\n", src_a.x, src_a.y, src_b.x, src_b.y);
                emxlogt("previous_center_p:(%d,%d)\n", previous_center_p.x, previous_center_p.y);
                emxlogt("current_center_p:(%d,%d)\n", current_center_p.x, current_center_p.y);
                double crossProduct = (src_b.x - src_a.x) * (previous_center_p.y - src_a.y) - (src_b.y - src_a.y) * (previous_center_p.x - src_a.x);

                direction = TopToBottom;
                if (crossProduct > 0) {
                    // 之前的点在下区域
                    direction = BottomToTop;
                } else if (crossProduct < 0) {
                    // 之前的点在上区域
                    direction = TopToBottom;
                } else {
                    // 该场景不应该存在
                    emxloge("this error point status\n");    
                }
            }

        private:
            int m_ax;int m_ay;int m_bx;int m_by;

    };
    public:
        EnvAlarmPassengerFlow() : EnvBase("alarmPassengerFlow") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool on;
        bool exposureOn;
        int exposureAdjust;
        SnapMode mode;
        int qualityValue;
        int sensitivity;
        DirectionRule statisticsMode; // 越界方式,0代表上出下进,1代表上进下出，默认值为0
        StatisticsLine line; // 规则线,将图形10000等分,A的x小于等于B的x,当A的x等于B的x时,A的y小于B的y
        EnvZone zone;
        bool osdStatus;
        int inCount = 0;  //不做文件配置存储
        int outCount = 0; //不做文件配置存储
    };

}

#endif //EAPIL_EnvAlarmPassengerFlow_HPP
