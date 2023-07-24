//
// Created by xiong on 2022/6/24.
//

#ifndef EMX_ThreadInvoke_HPP
#define EMX_ThreadInvoke_HPP

#include "EmxCore.hpp"

namespace Emx {

    class ThreadInvoke {
    public:
        struct Packet {
            Packet() {}

            Packet(uint32_t inId, void *inData) : id(inId), data(inData) {}

            uint32_t id;
            void *data;
            sem_t *sem;
        };

        class Req {
        public:
            Req(const char *respId);

            virtual ~Req();

            ErrCodeE Send(Packet &packet);

        private:
            SocketUnDgram m_sock;
            sockaddr_un m_addr;
        };

        class Resp {
        public:
            using Callback = std::function<void(Packet &packet)>;

            Resp(EuvLoop &loop) : m_loop(loop), m_started(false) {}

            virtual ~Resp() {}

            void Start(const char *respId, Callback cb);

            void Stop();

        private:
            int m_sock;
            EuvLoop &m_loop;
            EuvUdp m_udp;
            char m_buffer[256];
            Callback m_cb;
            bool m_started;
        };
    };
}

#endif //EMX_ThreadInvoke_HPP
