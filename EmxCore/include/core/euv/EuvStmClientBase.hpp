//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVSTMCLIENTBASE_HPP
#define EMX_EUVSTMCLIENTBASE_HPP

#include <memory>
#include "EuvStreamInf.hpp"

namespace Emx {
    /*!
     * @brief 面向连接的socket通信Client端基类，实现了这种类型客户端的创建销毁发送接收，
     * 内部使用EuvStreamInf抽象接口作为底层通信接口，实际实现客户端的时候，需要传入继承EuvStreamInf的实例
     * 并且最终的客户端需要继承EuvStmClientBase类，实现其各种虚函数。可参MsgRpcRep的实现
     * @see MsgRpcRep
     */
    class EuvStmClientBase {
    public:

        EuvStmClientBase(EuvStreamInf *stm) : m_stm(stm) {
            m_isConnected = false;
        }

        virtual ~EuvStmClientBase() {}

        /*!
         * @brief 创建客户端并连接服务端
         * @param loop [in] 需要绑定的loop
         * @param buffer [in] 需要传入一个buffer，用于接收数据
         * @param size [in] buffer的大小
         * @param serverIp [in] 需要连接的服务端ip
         * @param serverPort [in] 需要连接的服务端端口
         * @param localIp [in] 需要绑定的本地ip，默认不绑定本地ip
         * @param localPort [in] 需要绑定的本地端口，默认不绑定本地端口
         * @return ErrCodeE
         * @see ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop, char *buffer, int32_t size,
                        const char *serverIp, uint16_t serverPort,
                        const char *localIp = nullptr, uint16_t localPort = 0);

        //!@brief 断开链接并销毁客户端
        void Destroy();

        /*!
         * @brief 向服务端发送数据
         * @param data [in] 发送的数据，采用共享指针的方式，方便内存空间的回收
         * @return ErrCodeE
         */
        ErrCodeE Send(std::shared_ptr<std::string> &data);

        //!@brief 停止接收，调用后将不会收到数据
        void PauseRecv();

        /*!
         * @brief 开始接收数据/重新开始接收数据
         * @return ErrCodeE
         */
        ErrCodeE ResumeRecv();

        /*!
         * @brief 获取当前客户端是否正在监听接收
         * @return bool
         */
        bool IsReceiving() { return m_stm->IsReceiving(); }

        /*!
         * @brief 获取本地地址
         * @return 存储本地地址的string
         */
        std::string GetSockName() {
            char name[EMX_MAX_PATH_SIZE] = {};
            size_t size = EMX_MAX_PATH_SIZE;
            m_stm->GetSockName(name, size);
            return name;
        }

        /*!
         * @brief 获取对方地址
         * @return 存储对方地址的string
         */
        std::string GetPeerName() {
            char name[EMX_MAX_PATH_SIZE] = {};
            size_t size = EMX_MAX_PATH_SIZE;
            m_stm->GetPeerName(name, size);
            return name;
        }

    protected:

        //!@brief 当与服务端的链接建立成功后触发此函数的调用
        virtual void OnConnect() = 0;

        //!@brief 当与服务端的链接断开后触发此函数的调用
        virtual void OnDisConnect() = 0;

        /*!
         * @brief 当有数据到来时触发此函数的调用
         * @param data [in] 到来的数据
         * @param size [in] 到来的数据大小
         */
        virtual void OnRecv(const char *data, int32_t size) {};

        /*!
         * @brief 当调用Send函数发送的数据成功发送至服务端，或出现错误时触发次回调
         * @param e [in] 发送是否成功
         * @param buffer [in] 发送的数据
         * @param size [in] 发送的数据大小
         */
        virtual void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {};
    private:
        struct WriteReq {
            uv_write_t req;
            uv_buf_t buf;
            std::shared_ptr<std::string> data;
            EuvStmClientBase *obj;
        };
    private:
        bool m_isConnected;
        char *m_buffer;
        int32_t m_size;
        EuvStreamInf *m_stm;
    };
}

#endif //EMX_EUVSTMCLIENTBASE_HPP
