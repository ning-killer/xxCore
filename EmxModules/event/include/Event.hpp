/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_Event_HPP
#define EMX_Event_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Event {
    public:
        /**
         * @brief 事件类型
         */
        enum class TypeE {
            Input = 0, //!<事件输入
            Output, //!<事件输出
            Motion, //!<移动事件
            Person, //!<人行事件
            Tamper, //!<遮挡事件
            Invision, //!<区域入侵事件
        };

        /**
         * @brief 对外传递的事件信息
         */
        struct Info {
            int32_t chn; //!<通道号
            TypeE type; //!<事件类型
            bool isStart;//!<true:事件开始;false:事件结束
            uint64_t timestampMs; //!<时间戳
        };

        /**
         * @brief 支持的事件类型以及其通道数量
         */
        struct Channel {
            TypeE type; //!<支持的事件类型
            int chnNum; //此类事件支持的通道数量
        };
    };

}
#endif //EMX_Event_HPP
