//
// Created by xiong on 2021/7/19.
//

#ifndef EMX_LogServer_HPP
#define EMX_LogServer_HPP

#include <functional>
#include "core/EmxLog.hpp"
#include "uv.h"
#include "LogInner.hpp"

namespace Emx {

    class LogServer {
    public:

        /*!
         *
         * @param cfg the configuration of LogServer
         * @param pt point to the callback function of print
         */
        explicit LogServer();

        ~LogServer();

        /*!
         * Create and run LogServer
         */
        void Create();

        /*!
         * Stop and Destroy LogServer
         */
        void Destroy();

    private:
        struct Channel;

        void CreateLogFileChannel(Channel &chn, Log::Config::Channel &cfg);

        void DestroyLogFileChannel(Channel &chn);

        void CreateLogFile();

        void DestroyLogFile();

        static void OnQuit(uv_async_t *handle);

        static void OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

        static void OnRecvNormalLog(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                                    const struct sockaddr *addr, unsigned flags);

        static void OnRecvFlashLog(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                                   const struct sockaddr *addr, unsigned flags);

        static void OnRecvCtrl(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                               const struct sockaddr *addr, unsigned flags);

        void CmdRespResult(LogInner::CmdE cmd, ErrCodeE e, const struct sockaddr *addr,
                           void *data = nullptr, int32_t size = 0);

        void CreateShared();

        void CreateOutput();

        void DestroyOutput();

        void SendOutput(const char *log, int size);

    private:
        struct Channel {
            uv_udp_t sock;
            FILE *fp;
            int64_t size;
        };
        Channel m_normal;
        Channel m_flash;
        uv_loop_t *m_loop;
        uv_udp_t m_ctrl;
        uv_async_t m_quit;
        Log::Config m_cfg;
        Log::Config::Shared *m_shared;
        int m_shmId;
        void *m_shm;
        uv_thread_t m_thread;
        struct Legacy {
            bool ena;
            int32_t sock;
            sockaddr_un address;
        } m_output;
        bool m_created;
    };

}

#endif //EMX_LogServer_HPP
