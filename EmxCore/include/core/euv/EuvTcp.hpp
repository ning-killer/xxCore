//
// Created by xiong on 2020/11/6.
//

#ifndef EMX_EUVTCP_HPP
#define EMX_EUVTCP_HPP

#include "EuvLoop.hpp"
#include "EuvStmClientBase.hpp"
#include "EuvStmServerBase.hpp"

namespace Emx {

    //!@brief 以tcp为底层通信基础实现接口类EuvStreamInf。
    class EuvTcp : public EuvStreamInf {
    public:
        EuvTcp() : m_tcp(nullptr) {}

        virtual ~EuvTcp() {
            DoDestroy();
        }

        /*!
         * @brief 创建并初始化tcp
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop) override;

        //!@brief 销毁tcp
        void Destroy() override { DoDestroy(); }

        /*!
         * @brief 绑定此tcp到指定的本地地址
         * @param ip [in] 本地ip地址
         * @param port 本地端口
         * @return ErrCodeE
         */
        ErrCodeE Bind(const char *ip, uint16_t port) override;

        /*!
         * @brief 打开一个已经存在的fd
         * @param fd [in] 已经存在的fd
         * @return ErrCodeE
         */
        ErrCodeE Open(int fd) override;

        /*!
         * @brief 连接到服务端
         * @param req [in] 用于连接的libuv请求结构体
         * @param serverIp [in] 目的地址
         * @param serverPort [in] 目的端口
         * @param cb [in] 注册一个连接成功或失败后的回调
         * @return ErrCodeE
         */
        ErrCodeE Connect(uv_connect_t *req,
                         const char *serverIp, uint16_t serverPort,
                         uv_connect_cb cb) override;

        /*!
         * @brief 获取本地地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetSockName(void *name, size_t &len) override;

        /*!
         * @brief 获取对方地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetPeerName(void *name, size_t &len) override;

        /*!
         * @brief 获取libuv的stream句柄
         * @return libuv的stream句柄
         */
        uv_stream_t *GetStm() override { return (uv_stream_t *) m_tcp; }

        /*!
         * @brief 获取文件描述符fd
         * @return fd
         */
        int32_t GetFd() override { return m_tcp->io_watcher.fd; }

        /*!
         * @brief 获取当前描述符是否监听接收
         * @return bool
         */
        bool IsReceiving() override { return (m_tcp->flags & 0x00001000); }

    private:
        static void OnDestroy(uv_handle_t *handle);

        void DoDestroy();

    private:
        uv_tcp_t *m_tcp;
    };

    /*!
     * @brief 使用EuvTcp作为EuvStreamInf，并且继承EuvStmClientBase，
     * 形成以TCP为底层通信机制面向连接的客户端
     */
    class EuvTcpClientBase : public EuvStmClientBase {
    public:
        EuvTcpClientBase() : EuvStmClientBase(&m_tcp) {}

        virtual ~EuvTcpClientBase() {};

    private:
        EuvTcp m_tcp;
    };

    /*!
     * @brief 使用EuvTcp作为EuvStreamInf，并且继承EuvStmServerClientBase，
     * 形成以TCP为底层通信机制面向连接的ServerClient端
     */
    class EuvTcpServerClient : public EuvStmServerClientBase {
    public:
        EuvTcpServerClient(EuvStmServerHostBase *host) : EuvStmServerClientBase(&m_tcp, host) {}

        virtual ~EuvTcpServerClient() {};

    private:
        EuvTcp m_tcp;
    };

    /*!
     * @brief 使用EuvTcp作为EuvStreamInf，并且继承EuvStmServerHostBase，
     * 形成以TCP为底层通信机制面向连接的ServerHost端
     */
    class EuvTcpServerHost : public EuvStmServerHostBase {
    public:
        EuvTcpServerHost() : EuvStmServerHostBase(&m_tcp) {}

        virtual ~EuvTcpServerHost() {};

    private:
        EuvTcp m_tcp;
    };
}

#endif //EMX_EUVTCP_HPP
