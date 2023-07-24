/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventGlobal_HPP
#define EMX_EventGlobal_HPP

#include "Event.hpp"
#include "Schedule.hpp"

namespace Emx {

    class EventGlobal {
    public:
        /**
         * @brief 事件的全局参数
         */
        struct Param {
            void Serialize(Json::Value &json) {
                json["ena"] = ena;
                schedule.Serialize(json["schedule"]);
            }

            void Serialize(std::string &data) {
                data.append((char *) &ena, sizeof(ena));
                schedule.Serialize(data);
            }

            void Parse(const Json::Value &json) {
                ena = json["ena"].asBool();
                schedule.Parse(json["schedule"]);
            }

            int Parse(const char *data, int size) {
                int off = 0;
                memcpy(&ena, data + off, sizeof(ena));
                off += sizeof(ena);
                off += schedule.Parse(data + off, size - off);
                return off;
            }

        public:
            bool ena; //!<是否开启事件功能
            Schedule::Param schedule; //!<全局排期
        };


        /**
        * @brief 获取设备支持的事件通道信息
        * @param events [out] 支持的事件类型以及其通道数量
        * @return ErrCodeE
        */
        virtual ErrCodeE GetSupportedEventChannel(std::vector<Event::Channel> &channels) = 0;

        /**
         * @brief 获取全局参数
         * @param param [out] 参数数据数组
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(std::shared_ptr<Emx::EventGlobal::Param> &param) = 0;

        /**
        * @brief 设置全局参数
        * @param param [in] 参数数据数组
        * @return ErrCodeE
        */
        virtual ErrCodeE SetParam(const std::shared_ptr<Param> &param) = 0;
    };

}
#endif //EMX_EventGlobal_HPP
