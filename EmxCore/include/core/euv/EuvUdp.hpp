//
// Created by xiong on 2020/11/7.
//

#ifndef EMX_EUVUDP_HPP
#define EMX_EUVUDP_HPP

#include "EuvLoop.hpp"

namespace Emx {

    //!@brief 基于EuvLoop的UDP异步接收发送类
    class EuvUdp {
    public:
        /*!
         * @brief 当接收到数据后会调用此回调函数
         * @param e [in] 接收错误信息
         * @param data [in] 接收到的数据指针
         * @param size [in] 接收到的数据大小
         * @param addr [in] 对方的地址
         */
        using RecvCallback = std::function<void(Emx::ErrCodeE e, const char *data, int32_t size,
                                                const struct sockaddr *addr)>;

        EuvUdp() : m_udp(nullptr) {}

        virtual ~EuvUdp() {
            Destroy();
        }

        /*!
         * @brief 创建UDP并绑定到loop
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop);

        //!@brief 销毁UDP
        void Destroy();

        /*!
         * @brief 可以使用一个已经打开的socket上
         * @param sock 需要使用的socket
         * @return ErrCodeE
         */
        ErrCodeE OpenExistSock(uv_os_sock_t sock);

        /*!
         * @brief 创建一个socket并绑定到本地的地址
         * @param addr [in] 地址
         * @param flags [in] 同bind函数
         * @return ErrCodeE
         */
        ErrCodeE Bind(const struct sockaddr *addr, unsigned int flags);

        /*!
         * @brief 使能广播
         * @param ena [in] 是否使能
         * @return ErrCodeE
         */
        ErrCodeE SetBroadcast(bool ena);

        /*!
         * @brief 开始接收数据
         * @param buffer [in] 接收数据所使用的buffer指针
         * @param size [in] 接受数据所使用的buffer大小
         * @param cbRecv [in] 当数据到来时的回调函数
         * @return ErrCodeE
         */
        ErrCodeE StartRecv(char *buffer, int32_t size, RecvCallback cbRecv);

        /*!
         * @brief 以阻塞的方式发送数据
         * @param data [in] 发送的数据指针
         * @param size [in] 发送的数据大小
         * @param addr [in] 发送的目的地址
         * @param addrLen [in] 发送的目的地址结构体长度
         * @return 同sendto函数
         */
        int32_t Send(uint8_t *data, int32_t size, const struct sockaddr *addr, socklen_t addrLen) {
            return sendto(m_udp->io_watcher.fd, data, size, 0, addr, addrLen);
        }

        /*!
         * @brief 获取对端A地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetPeerName(struct sockaddr *name, int *len);

        /*!
         * @brief 获取本地地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetSockName(struct sockaddr *name, int *len);

        /*!
         * @brief 获取发送等待队列中的数据大小
         * @return 数据大小
         */
        size_t GetSendQueueSize() { return uv_udp_get_send_queue_size(m_udp); }

        /*!
         * @brief 获取发送等待队列的个数
         * @return 发送队列等待的个数
         */
        size_t GetSendQueueCount() { return uv_udp_get_send_queue_count(m_udp); }

        /*!
         * @brief 获取libuv的udp句柄
         * @return udp句柄
         */
        uv_udp_t *GetUDP() { return m_udp; }

        /*!
         * @brief 获取文件描述符fd
         * @return fd
         */
        int32_t GetFD() { return m_udp->io_watcher.fd; }

        /*!
         * @brief 获取当前描述符是否监听接收
         * @return bool
         */
        bool IsReceiving() { return (m_udp->flags & 0x00001000); }

    private:
        static void OnDestroy(uv_handle_t *handle);

    private:
        uv_udp_t *m_udp;
        RecvCallback m_cbRecv;
        char *m_buffer;
        int32_t m_size;
    };
}
#endif //EMX_EUVUDP_HPP
