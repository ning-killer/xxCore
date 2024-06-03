//
// Created by xiong on 2020/10/29.
//

#ifndef EMX_EUVPOLL_HPP
#define EMX_EUVPOLL_HPP

#include "EuvLoop.hpp"

namespace Emx {
    /*!
     * @brief EuvPoll可以实现所有文件描述符的监听，可以实现读写和连接状态的监听，
     * 当某些描述符并不是文件或者socket或者描述符已经被打开的时候可以考虑使用这个，
     * 例如海思中获取venc的时候使用的HI_S32 HI_MPI_VENC_GetFd(VENC_CHN VeChn)函数获取的fd
     */
    class EuvPoll {
    public:

        //!@brief 当有事件触发时调用此回调
        using Callback = std::function<void()>;

        EuvPoll() {}

        /*!
         * @brief 初始化poll，并绑定到loop上，当fd非socket类型时使用
         * @param loop [in] 需要绑定的loop
         * @param fd [in] 需要监听的fd
         * @return ErrCodeE
         */
        ErrCodeE Init(EuvLoop &loop, int fd);

        /*!
         * @brief 初始化poll，并绑定到loop上，当fd是socket类型时使用
         * @param loop [in] 需要绑定的loop
         * @param fd [in] 需要监听的fd
         * @return ErrCodeE
         */
        ErrCodeE InitSock(EuvLoop &loop, int fd);

        /*!
         * @brief 开始监听
         * @param OnRead [in] 当fd可读时触发此回调，null表示不监听此事件
         * @param OnWrite [in] 当fd可写时触发此回调，null表示不监听此事件
         * @param OnDisconnect [in] 当fd断开链接时触发此回调，null表示不监听此事件
         * @return ErrCodeE
         */
        ErrCodeE Start(Callback OnRead, Callback OnWrite, Callback OnDisconnect);

        //!@brief 停止监听
        void Stop() { uv_poll_stop(&m_poll); }

        /*!
         * @brief 获取libuv的handle
         * @return handle
         */
        uv_handle_t *GetHandle() { return (uv_handle_t *) &m_poll; }

    private:
        uv_poll_t m_poll;
        Callback m_cbRead;
        Callback m_cbWrite;
        Callback m_cbDisconnect;
    };
}


#endif //EMX_EUVPOLL_HPP
