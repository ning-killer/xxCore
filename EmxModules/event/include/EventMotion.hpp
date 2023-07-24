/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventMotion_HPP
#define EMX_EventMotion_HPP

#include "Event.hpp"
#include "Zone.hpp"

namespace Emx {

    class EventMotion {
    public:
        EventMotion(int chn) : m_chn(chn) {}

        virtual ~EventMotion() {}

        struct Param {
            void Serialize(Json::Value &json) {
                json["ena"] = ena;
                json["sensitivity"] = sensitivity;
                zone.Serialize(json["zone"]);
            }

            void Serialize(std::string &data) {
                data.append((char *) &ena, sizeof(ena));
                data.append((char *) &sensitivity, sizeof(sensitivity));
                data.append((char *) zone.map, sizeof(zone.map));
            }

            void Parse(const Json::Value &json) {
                ena = json["ena"].asBool();
                sensitivity = json["sensitivity"].asInt();
                zone.Parse(json["zone"]);
            }

            void Parse(const char *data, int size) {
                int off = 0;
                memcpy(&ena, data + off, sizeof(ena));
                off += sizeof(ena);
                memcpy(&sensitivity, data + off, sizeof(sensitivity));
                off += sizeof(sensitivity);
                memcpy(zone.map, data + off, sizeof(zone.map));
                off += sizeof(zone.map);
            }

        public:
            bool ena;
            uint8_t sensitivity;
            Zone zone;
        };

        /**
         * @brief 根据事件类型获取其参数
         * @param chn [in] 事件通道
         * @param param [out] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(std::shared_ptr<Param> &param) = 0;

        /**
        * @brief 根据事件类型设置其参数
        * @param chn [in] 事件通道
        * @param param [in] 参数数据
        * @return ErrCodeE
        */
        virtual ErrCodeE SetParam(const std::shared_ptr<Param> &param) = 0;

        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };

}
#endif //EMX_EventMotion_HPP
