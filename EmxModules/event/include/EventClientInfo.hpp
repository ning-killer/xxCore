/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventClientInfo_HPP
#define EMX_EventClientInfo_HPP

#include "Event.hpp"

namespace Emx {

    class EventClientInfo {
    public:
        EventClientInfo() : m_started(false) {}

        using OnGetInfoCallback = std::function<void(Event::Info &info)>;

        /**
         * @brief 开始接收事件消息数据
         * @param loop [in] loop
         * @param buffer [in] 提供一个buffer
         * @param bufferSize [in] 提供的buffer大小
         * @param cb [in] 获取到数据后的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Start(EuvLoop &loop, char *buffer, int bufferSize, OnGetInfoCallback cb);

        //!@brief 停止接收事件消息数据
        void Stop();

    protected:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        MsgTopicSubscriberAsync m_sub;
        OnGetInfoCallback m_cb;
        bool m_started;
    };

}

#endif //EMX_EventClientInfo_HPP
