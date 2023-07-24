//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVPIPE_HPP
#define EMX_EUVPIPE_HPP

#include "EuvLoop.hpp"
#include "EuvStmClientBase.hpp"
#include "EuvStmServerBase.hpp"

namespace Emx {

    //!@brief 以pipe为底层通信基础实现接口类EuvStreamInf，pipe的底层使用STREAM+AF_UNIX的方式进行通信。
    class EuvPipe : public EuvStreamInf {
    public:
        EuvPipe() : m_pipe(nullptr) {}

        virtual ~EuvPipe() {
            DoDestroy();
        }

        /*!
         * @brief 创建并初始化Pipe
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop) override;

        //!@brief 销毁Pipe
        void Destroy() override { DoDestroy(); }

        /*!
         * @brief 绑定此Pipe到指定的本地地址
         * @param ip [in] 这里的参数应填写AF_UNIX格式的地址，例如/tmp/domain/msg/send_pipe
         * @param port 此参数无效，不会使用
         * @return ErrCodeE
         */
        ErrCodeE Bind(const char *ip, uint16_t port) override;

        /*!
         * @brief 连接到服务端
         * @param req [in] 用于连接的libuv请求结构体
         * @param serverIp [in] 目的AF_UNIX地址
         * @param serverPort [in] 无效，不会使用
         * @param cb [in] 注册一个连接成功或失败后的回调
         * @return ErrCodeE
         */
        ErrCodeE Connect(uv_connect_t *req,
                         const char *serverIp, uint16_t serverPort,
                         uv_connect_cb cb) override;

        /*!
         * @brief 获取本地AF_UNIX地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetSockName(void *name, size_t &len) override;

        /*!
         * @brief 获取对端AF_UNIX地址
         * @param name [out] 存储地址的buffer
         * @param len [in,out] 存储地址的buffer最大大小，返回实际地址长度
         * @return ErrCodeE
         */
        ErrCodeE GetPeerName(void *name, size_t &len) override;

        /*!
         * @brief 获取libuv的stream句柄
         * @return libuv的stream句柄
         */
        uv_stream_t *GetStm() override { return (uv_stream_t *) m_pipe; }

        /*!
         * @brief 获取文件描述符fd
         * @return fd
         */
        int32_t GetFd() override { return m_pipe->io_watcher.fd; }

        /*!
         * @brief 获取当前描述符是否监听接收
         * @return bool
         */
        bool IsReceiving() override { return (m_pipe->flags & 0x00001000); }

    private:
        static void OnDestroy(uv_handle_t *handle);

        void DoDestroy();

    private:
        uv_pipe_t *m_pipe;
    };

    /*!
     * @brief 使用EuvPipe作为EuvStreamInf，并且继承EuvStmClientBase，
     * 形成以Pipe为底层通信机制面向连接的客户端
     */
    class EuvPipeClientBase : public EuvStmClientBase {
    public:
        EuvPipeClientBase() : EuvStmClientBase(&m_pipe) {}

        virtual ~EuvPipeClientBase() {};
    private:
        EuvPipe m_pipe;
    };

    /*!
     * @brief 使用EuvPipe作为EuvStreamInf，并且继承EuvStmServerClientBase，
     * 形成以Pipe为底层通信机制面向连接的ServerClient端
     */
    class EuvPipeServerClient : public EuvStmServerClientBase {
    public:
        EuvPipeServerClient(EuvStmServerHostBase *host) : EuvStmServerClientBase(&m_pipe, host) {}

        virtual ~EuvPipeServerClient() {};

    private:
        EuvPipe m_pipe;
    };

    /*!
     * @brief 使用EuvTcp作为EuvStreamInf，并且继承EuvStmServerHostBase，
     * 形成以TCP为底层通信机制面向连接的ServerHost端
     */
    class EuvPipeServerHost : public EuvStmServerHostBase {
    public:
        EuvPipeServerHost() : EuvStmServerHostBase(&m_pipe) {}

        virtual ~EuvPipeServerHost() {};

    private:
        EuvPipe m_pipe;
    };

}


#endif //EMX_EUVPIPE_HPP
