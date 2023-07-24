/*
 * @Author: xiong
 * @Date: 2022/9/6
*/

#ifndef EMX_UpdateClient_HPP
#define EMX_UpdateClient_HPP

#include "EmxCore.hpp"

namespace Emx {
    class UpdateClient {
    public:
        UpdateClient() {
            m_created = false;
            m_buffer = nullptr;
        }

        ErrCodeE Create(uint32_t session);

        ErrCodeE Push(const uint8_t *data, int32_t size);

        void Destroy();

    private:
        bool m_created;
        uint8_t *m_buffer;
        uint32_t m_session;
        uint32_t m_seq;
        SocketUnDgram m_sock;
        sockaddr_un m_addr;
    };

    class UpdateEvent {
    public:
        enum class StatE : uint8_t {
            Idle = 0,
            Start,
            Done,
            Error,
        };

        struct Event {
            StatE stat;
            uint32_t session;
            struct Size {
                int32_t total;
                int32_t current;
            } download, burn;
        };
        using EventCallBack = std::function<void(Event &e)>;

        ErrCodeE Create(EuvLoop *loop, char *buffer, int size, EventCallBack cb);

        void Destroy();

    private:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        MsgTopicSubscriberAsync m_sub;
        EventCallBack m_cb;
    };
};

#endif //EMX_UpdateClient_HPP
