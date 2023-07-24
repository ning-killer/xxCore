//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVSTREAMINF_HPP
#define EMX_EUVSTREAMINF_HPP

#include "EuvLoop.hpp"

namespace Emx {

    //It's the abstract interface for stream-like interfaces eg. pipe/tcp
    //!@brief 面向链接类型的抽象接口(例如tcp/pipe)，用于为EuvStmClientBase/EuvStmServerHostBase等提供统一的底层通信接口
    class EuvStreamInf {
    public:

        /*!
         * @brief 创建接口
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        virtual ErrCodeE Create(EuvLoop &loop) = 0;

        //!@brief 销毁接口
        virtual void Destroy() = 0;

        /*!
         * @brief 绑定接口到本地的地址
         * @param ip [in] 本地ip
         * @param port [in] 本地port
         * @return ErrCodeE
         */
        virtual ErrCodeE Bind(const char *ip, uint16_t port) { return ErrCodeE::OperationNotSupport; };

        /*!
         * @brief 打开一个已经存在的fd
         * @param fd [in] 已经存在的fd
         * @return ErrCodeE
         */
        virtual ErrCodeE Open(int fd) { return ErrCodeE::OperationNotSupport; };

        /*!
         * @brief 连接到服务器
         * @param req [in] 发起请求的时候携带的libuv中的请求句柄
         * @param serverIp [in] 目的server地址
         * @param serverPort [in] 目的server端口
         * @param cb [in] 连接成功或失败后的回调函数
         * @return ErrCodeE
         */
        virtual ErrCodeE Connect(uv_connect_t *req,
                                 const char *serverIp, uint16_t serverPort,
                                 uv_connect_cb cb) { return ErrCodeE::OperationNotSupport; }

        /*!
         * @brief 获取本地地址
         * @param name [out] 存放地址的buffer
         * @param len [in,out] 存放地址buffer的大小，返回实际的地址大小
         * @return ErrCodeE
         */
        virtual ErrCodeE GetSockName(void *name, size_t &len) = 0;

        /*!
         * @brief 获取对方地址
         * @param name [out] 存放地址的buffer
         * @param len [in,out] 存放地址buffer的大小，返回实际的地址大小
         * @return ErrCodeE
         */
        virtual ErrCodeE GetPeerName(void *name, size_t &len) = 0;

        /*!
         * @brief 获取libuv的stream句柄
         * @return libuv的stream句柄
         */
        virtual uv_stream_t *GetStm() = 0;

        /*!
         * @brief 获取文件描述符fd
         * @return fd
         */
        virtual int32_t GetFd() = 0;

        /*!
         * @brief 获取当前描述符是否监听接收
         * @return bool
         */
        virtual bool IsReceiving() = 0;
    };
}


#endif //EMX_EUVSTREAMINF_HPP
