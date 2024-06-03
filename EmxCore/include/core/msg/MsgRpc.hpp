//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_MSGRPC_HPP
#define EMX_MSGRPC_HPP

#include "core/EmxUV.hpp"

namespace Emx {

    //!@brief 用于MsgRpc通信的数据类型定义
    class MsgRpc {
    public:

        //!@brief RPC客户端类型
        enum class ClientTypeE : uint8_t {
            Req = 0, //!<请求端
            Resp //!<响应端
        };

        //!@brief RPC请求的数据协议头
        struct Header {
            uint32_t sync;
            uint32_t reqClientId;
            uint32_t respClientId;
            uint32_t sessionId;
            uint32_t module;
            uint32_t method;
            ErrCodeE code;
            uint32_t size;
        };
    };

    //!@brief 同步阻塞RPC请求
    class MsgRpcReqSync {
    public:
        /*!
         * @brief 发起同步阻塞RPC请求，比较耗时耗资源，尽量使用MsgRpcReqAsync异步请求
         * @param dstClientId [in] 响应端的ClientID，由响应端定义
         * @param module [in] 目标模块，由响应端定义
         * @param method [in] 目标方法，由响应端定义
         * @param req [in] 请求携带的数据，默认不携带数据
         * @param size [in] 请求携带数据大小，默认携带数据为0
         * @param resp [out] 响应返回的数据，默认不需要返回数据
         * @param timeoutMs [in] 请求超时时间，默认5s
         * @return ErrCodeE 请求端/响应端/请求转发端返回的错误码
         */
        static ErrCodeE Request(uint32_t dstClientId,
                                uint32_t module, uint32_t method,
                                const char *req = nullptr, int32_t size = 0,
                                std::string *resp = nullptr, int32_t timeoutMs = 5000);
    };

    //!@brief 异步非阻塞RPC请求
    class MsgRpcReqAsync : public EuvPipeClientBase {
    public:

        //!@brief 连接状态回调
        using Callback = std::function<void()>;

        /*!
         * @brief 接收到响应数据回调
         * @param e [in] 请求端/响应端/请求转发端返回的错误码
         * @param clientId [in] 接收到的响应端的clientID
         * @param module [in] 接收到的响应端的模块
         * @param method [in] 接收到的响应端的方法
         * @param data [in] 接收到的响应数据
         * @param size [in] 接收到的响应数据大小
         */
        using RecvRespCallback = std::function<void(Emx::ErrCodeE e, uint32_t clientId,
                                                    uint32_t module, uint32_t method,
                                                    const char *data, int32_t size)>;

        //!@brief 启动异步请求时携带的配置参数
        struct Config {
            EuvLoop *loop; //!<需要绑定的loop
            char *buffer; //!<需要提供一个buffer来接收数据
            int32_t size; //!<buffer的大小
            Callback OnConnected; //!<注册一个当与MsgRpcServer成功建立连接后的回调，连接成功后才能发送请求
            Callback OnDisConnected; //!<注册一个当与MsgRpcServer断开连接后的回调
            RecvRespCallback OnRecvResp; //!<注册一个用于接收响应消息的回调
        };

        MsgRpcReqAsync() : EuvPipeClientBase(), m_header({}) {
            m_waitSize = sizeof(MsgRpc::Header);
            m_waitHeader = true;
        }

        /*!
         * @brief 创建异步消息请求体
         * @param cfg [in] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE CreateMsgRpcReqAsync(Config &cfg);

        //!@brief 销毁请求体
        void DestroyMsgRpcReqAsync() { Destroy(); }

        /*!
         * @brief 发起异步请求
         * @param dstClientId [in] 响应端的ClientID，由响应端定义
         * @param module [in] 目标模块，由响应端定义
         * @param method [in] 目标方法，由响应端定义
         * @param req [in] 请求携带的数据，默认不携带数据
         * @param size [in] 请求携带数据大小，默认携带数据为0
         * @return ErrCodeE 请求端/响应端/请求转发端返回的错误码
         */
        ErrCodeE Request(uint32_t dstClientId, uint32_t module, uint32_t method,
                         const char *req = nullptr, int32_t size = 0);

    protected:

        void OnConnect() override { if (m_cbOnConnected)m_cbOnConnected(); }

        void OnDisConnect() override { if (m_cbOnDisConnected)m_cbOnDisConnected(); }

        void OnRecv(const char *data, int32_t size) override;

    private:
        bool m_waitHeader;
        uint32_t m_waitSize;
        MsgRpc::Header m_header;
        std::string m_data;
        Callback m_cbOnConnected;
        Callback m_cbOnDisConnected;
        RecvRespCallback m_cbOnRecvResp;
    };

    //!@brief 异步非阻塞响应端
    class MsgRpcRep : public EuvPipeClientBase {
    public:

        //!@brief 连接状态回调
        using Callback = std::function<void()>;
        /*!
         * @brief 接收到RPC请求回调
         * @param module [in] 请求的模块
         * @param method [in] 请求的方法
         * @param data [in] 请求携带的数据
         * @param size [in] 请求携带的数据大小
         * @param out [out] 返回的数据
         */
        using RecvReqCallback = std::function<ErrCodeE(uint32_t module, uint32_t method,
                                                       const char *data, int32_t size,
                                                       std::string &out)>;
        //!@brief 启动异步响应监听时携带的配置参数
        struct Config {
            EuvLoop *loop; //!<需要绑定的loop
            uint32_t clientId; //!<此响应端的ClientID
            char *buffer; //!<需要提供一个buffer来接收数据
            int32_t size; //!<buffer的大小
            Callback OnConnected; //!<注册一个当与MsgRpcServer成功建立连接后的回调
            Callback OnDisConnected; //!<注册一个当与MsgRpcServer断开连接后的回调
            RecvReqCallback OnRecvReq; //!<注册一个用于接收请求消息的回调
        };

        MsgRpcRep() : EuvPipeClientBase() {
            m_waitSize = sizeof(MsgRpc::Header);
            m_waitHeader = true;
            m_clientId = 0;
        }

        /*!
         * @brief 创建异步消息响应体
         * @param cfg [in] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE CreateMsgRpcRep(Config &cfg);

        //!@brief 销毁请求体
        void DestroyMsgRpcRep() { Destroy(); }

    protected:

        void OnConnect() override { if (m_cbConnected)m_cbConnected(); }

        void OnDisConnect() override { if (m_cbDisConnected)m_cbDisConnected(); }

        void OnRecv(const char *data, int32_t size) override;

    private:
        bool m_waitHeader;
        uint32_t m_waitSize;
        MsgRpc::Header m_header;
        std::string m_data;
        Callback m_cbConnected;
        Callback m_cbDisConnected;
        RecvReqCallback m_cbRecvReq;
        uint32_t m_clientId;
    };
}
#endif //EMX_MSGRPC_HPP
