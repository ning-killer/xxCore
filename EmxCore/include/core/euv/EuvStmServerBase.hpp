//
// Created by xiong on 2021/12/9.
//

#ifndef EMX_EuvStmServerBase_HPP
#define EMX_EuvStmServerBase_HPP

#include <list>
#include "EuvStreamInf.hpp"

namespace Emx {
    class EuvStmServerClientBase;

    /*!
     * @brief 面向连接的socket通信ServerHost端基类，是服务端的监听socket，实现了这种类型ServerHost端的创建销毁发送接收，
     * 内部使用EuvStreamInf抽象接口作为底层通信接口，实际实现服务端的时候，需要传入继承EuvStreamInf的实例
     * 并且最终的服务端需要继承EuvStmCServerHostBase类，实现其各种虚函数。可参MsgRpcServer的实现
     * @see MsgRpcServer
     */
    class EuvStmServerHostBase {
    public:

        EuvStmServerHostBase(EuvStreamInf *stm) : m_stm(stm), m_buffer(nullptr) {}

        virtual ~EuvStmServerHostBase() { Destroy(); }

        /*!
         * @brief 创建服务端并监听连接
         * @param loop [in] 需要绑定的loop
         * @param ip [in] 需要绑定的本地ip，默认不绑定本地ip
         * @param port [in] 需要绑定的本地端口，默认不绑定本地端口
         * @param maxClientNum [in] 监听的最大连接数
         * @param buffer [in] 需要传入一个buffer，用于接收数据
         * @param size [in] buffer的大小
         * @return ErrCodeE
         * @see ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop, const char *ip, uint16_t port, int maxClientNum, char *buffer, int32_t size);

        /*!
         * @brief 停止监听并销毁
         */
        void Destroy();

        /*!
         * @brief 主动断开一个客户端的链接，断开后会触发OnClientDisConnect回调
         * @param client [in] accept后的ServerClient客户端
         */
        void DisConnectClient(EuvStmServerClientBase *client);

    protected:

        /*!
         * @brief 当有客户端到来时，需要分配一个继承自EuvStmServerClientBase的ServerClient客户端给ServerHost
         * @note 分配的对象内存空间应由子类管理，EuvStmServerHostBase不会对此回调分配的对象内存空间的释放进行管理，
         * 子类可以在OnClientDisConnect回调中或是回调后对EuvStmServerClientBase对象的内存进行回收。
         * OnAllocClient后的对象有可能并不经历OnClientConnect而直接断开触发OnClientDisConnect
         * @return 分配的对象指针
         */
        virtual EuvStmServerClientBase *OnAllocClient() = 0;

        /*!
         * @brief 当客户端成功连接时触发此回调
         * @param client [in] 客户端指针(由OnAllocClient分配)
         */
        virtual void OnClientConnect(EuvStmServerClientBase *client) = 0;

        /*!
         * @brief 当客户端断开链接时触发此回调
         * @param client [in] 客户端指针(由OnAllocClient分配)
         */
        virtual void OnClientDisConnect(EuvStmServerClientBase *client) = 0;

        /*!
         * @brief 当某个ServerClient接收到数据时触发此回调
         * @param client [in] 客户端指针(由OnAllocClient分配)
         * @param data [in] 接收到的数据指针
         * @param size [in] 接收到的数据大小
         */
        virtual void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) = 0;

    private:
        EuvLoop *m_loop;
        EuvStreamInf *m_stm;
        char *m_buffer;
        int32_t m_size;
        std::list<EuvStmServerClientBase *> m_clientList;
    };

    /*!
     * @brief 面向连接的socket通信ServerClient端基类，是服务端accept后的socket，
     * 实现了这种类型ServerClient端的创建销毁发送接收，
     * 内部使用EuvStreamInf抽象接口作为底层通信接口，实际实现服务端的时候，需要传入继承EuvStreamInf的实例
     * 并且最终的服务端需要继承EuvStmCServerClientBase类，实现其各种虚函数。可参MsgRpcClient的实现
     * @see MsgRpcClient
     */
    class EuvStmServerClientBase {
    public:
        EuvStmServerClientBase(EuvStreamInf *stm, EuvStmServerHostBase *host) :
                m_stm(stm), m_host(host) {
            m_sendByPack = false;
            m_maxSend = -1;
            m_curSendSize = 0;
            m_curSendPack = 0;
        };

        virtual ~EuvStmServerClientBase() {};

        /*!
         * @brief 向客户端发送数据
         * @param data [in] 发送的数据，采用共享指针的方式，方便内存空间的回收和重复利用
         * @return ErrCodeE
         */
        ErrCodeE Send(std::shared_ptr<std::string> &data);

        /*!
         * @brief 调用Send发送的数据是需要等待发送缓冲区可用的，此函数负责设置当缓冲区满的情况下最多缓存的数据量
         * @param sendByPack [in] true:按照包数统计;false:按照字节数统计
         * @param maxSend [in] 最大缓存包数/字节数
         */
        void SetMaxSendBuffer(bool sendByPack, int32_t maxSend) {
            m_sendByPack = sendByPack;
            m_maxSend = maxSend;
        };

        /*!
         * @brief 获取ServerHost的指针
         * @return ServerHost的指针
         */
        EuvStmServerHostBase *GetHost() { return m_host; }

        /*!
         * @brief 获取EuvStreamInf
         * @return EuvStreamInf
         */
        EuvStreamInf *GetStmInf() { return m_stm; }

        /*!
         * @brief 获取当前需要发送的包数
         * @return 当前需要发送的包数
         */
        int32_t GetCurBufferedPack() { return m_curSendPack; }

        /*!
         * @brief 获取当前需要发送的字节数
         * @return 当前需要发送的字节数
         */
        int32_t GetCurBufferedSize() { return m_curSendSize; }

        /*!
         * @brief 当调用Send发送的数据发送完毕，或是出现错误时触发此回调
         * @param e [in] 错误信息
         * @param buffer [in] 发送结束的数据指针
         * @param size [in] 发送结束的数据大小
         */
        virtual void OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {};

    private:
        void SendInc(int32_t size) {
            m_curSendPack++;
            m_curSendSize += size;
        }

        void SendDec(int32_t size) {
            m_curSendPack--;
            m_curSendSize -= size;
        }

        bool BufferOverflow() const {
            if (m_maxSend != -1) {
                return m_sendByPack ? m_curSendPack > m_maxSend : m_curSendSize > m_maxSend;
            }
            return false;
        }

        struct WriteReq {
            uv_write_t req;
            uv_buf_t buf;
            std::shared_ptr<std::string> data;
            EuvStmServerClientBase *obj;
        };

    private:
        bool m_sendByPack;//buffer size calculate by pack number or total stream size
        int32_t m_maxSend;//limited send buffer size
        int32_t m_curSendSize;//current stream size
        int32_t m_curSendPack;//current pack num
        EuvStreamInf *m_stm;
        EuvStmServerHostBase *m_host;
    };
}

#endif //EMX_EuvStmServerBase_HPP
