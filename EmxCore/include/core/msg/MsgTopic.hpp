//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_MSGTOPIC_HPP
#define EMX_MSGTOPIC_HPP

#include <vector>
#include "core/EmxTypeDef.hpp"
#include "core/EmxUV.hpp"
#include "core/utils/Socket.hpp"

namespace Emx {

    //!@brief 用于Topic通信的数据类型定义
    class MsgTopic {
    public:

        //!@brief Topic客户端类型
        enum class ClientTypeE : uint8_t {
            Pub = 0, //!<发布端
            Sub //!<订阅端
        };

        //!@brief 客户端消息类型
        enum class MsgTypeE : uint8_t {
            Transfer = 0, //!<消息传送
            Register, //!<客户端注册
        };

        //!@brief 单个订阅端可订阅最大Topic数量
        static const int32_t MaxTopicNum = 64;

        //!@brief Topic数据协议头
        struct Header {
            uint32_t sync;
            MsgTypeE msgType;
            uint32_t size;
        };

        /*!
         * @brief 订阅端注册用结构体
         * @note 每个订阅端在TopicServer上都对应着一个发送端，可以通过sendByPack和maxSend字段控制这个发送端
         * 缓存的大小，缓存过大当订阅端处理不即时时可能会占用很多内存，分配过小可能导致丢掉一些订阅信息
         */
        struct SubRegister {
            Header header; //!< Topic数据协议头
            bool sendByPack; //!< true代表根据包的数量判断，false代表根据字节数判断
            int32_t maxSend; //!< TopicServer针对此订阅端缓存的最大数据量，单位由‘sendByPack’决定，-1表示无限制
            uint32_t topicArray[MaxTopicNum]; //!<需要订阅的topic，数组中以0作为有效topic的截止
        };

        /*!
         * @brief 用于消息传输的消息头
         */
        struct Transfer {
            Header header; //!<消息头
            uint32_t topic; //!<传送的topic
        };
    };

    /*!
     * @brief 用于Topic的异步发布
     */
    class MsgTopicPublisherAsync : public EuvPipeClientBase {
    public:
        /*!
         * @brief 连接状态回调
         */
        using Callback = std::function<void()>;

        /*!
         * @brief 启动异步发布时携带的配置参数
         */
        struct Config {
            EuvLoop *loop; //!<需要绑定的loop
            Callback OnConnected; //!<注册一个当与MsgTopicServer成功建立连接后的回调，连接成功后才能发布消息
            Callback OnDisConnected; //!<注册一个当与MsgTopicServer断开连接后的回调
        };

        MsgTopicPublisherAsync() : EuvPipeClientBase() {
            m_connected = false;
        }

        /*!
         * @brief 创建异步Topic发布
         * @param cfg [in] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE CreateMsgTopicPublisherAsync(const Config &cfg);

        /*!
         * @brief 销毁异步Topic发布
         */
        void DestroyMsgTopicPublisherAsync() { Destroy(); }

        /*!
         * @brief 发布一条Topic
         * @param topic [in] 消息的topic
         * @param data [in] topic携带的数据
         * @param size [in] topic携带数据的大小
         * @return ErrCodeE
         */
        ErrCodeE Publish(uint32_t topic, const char *data, int32_t size);

    protected:
        void OnConnect() override {
            m_connected = true;
            if (m_cbConnected)m_cbConnected();
        }

        void OnDisConnect() override {
            m_connected = false;
            if (m_cbDisConnected)m_cbDisConnected();
        }

    private:
        bool m_connected;
        Callback m_cbConnected;
        Callback m_cbDisConnected;
    };

    /*!
     * @brief 同步阻塞发布消息
     */
    class MsgTopicPublisherSync {
    public:
        /*!
         * @brief 发送同步阻塞消息，比较耗时耗资源，尽量使用MsgTopicPublisherAsync异步请求
         * @param topic [in] 消息的topic
         * @param data [in] topic携带的数据
         * @param size [in] topic携带数据的大小
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCodeE
         */
        static ErrCodeE Publish(uint32_t topic, const char *data, int32_t size, int32_t timeoutMs = 1000);
    };

    /*!
     * @brief 用于Topic的异步订阅
     */
    class MsgTopicSubscriberAsync : public EuvPipeClientBase {
    public:
        /*!
          * @brief 连接状态回调
          */
        using Callback = std::function<void()>;

        /*!
         * @brief 接收到订阅消息的回调函数
         * @param topic [in] 消息的topic
         * @param data [in] topic携带的数据
         * @param size [in] topic携带数据的大小
         */
        using RecvMsgCallback = std::function<void(uint32_t topic, const char *data, int32_t size)>;

        /*!
         * @brief 启动异步订阅时携带的配置参数
         * @note 每个订阅端在TopicServer上都对应着一个发送端，可以通过sendByPack和maxSend字段控制这个发送端
         * 缓存的大小，缓存过大当订阅端处理不即时时可能会占用很多内存，分配过小可能导致丢掉一些订阅信息
         */
        struct Config {
            EuvLoop *loop; //!<需要绑定的loop
            bool sendByPack; //!< true代表根据包的数量判断，false代表根据字节数判断
            int32_t maxSend; //!< TopicServer针对此订阅端缓存的最大数据量，单位由‘sendByPack’决定，-1表示无限制
            std::vector<uint32_t> topicArray; //!<需要订阅的topic数组
            char *buffer; //!<需要提供一个buffer来接收数据
            int32_t size; //!<buffer的大小
            Callback OnConnected; //!<注册一个当与MsgTopicServer成功建立连接后的回调
            Callback OnDisConnected; //!<注册一个当与MsgTopicServer断开连接后的回调
            RecvMsgCallback OnRecvMsg; //!<注册一个用于接收响应消息的回调
        };

        MsgTopicSubscriberAsync() : EuvPipeClientBase() {
            m_waitSize = sizeof(MsgTopic::Header);
            m_waitHeader = true;
            m_reg = nullptr;
        }

        /*!
         * @brief 创建异步订阅
         * @param cfg [in] 配置参数
         * @return ErrCodeE
         */
        ErrCodeE CreateMsgTopicSubscriberAsync(const Config &cfg);

        /*!
         * @brief 销毁异步订阅
         */
        void DestroyMsgTopicSubscriberAsync() { Destroy(); }

    protected:
        void OnConnect() override;

        void OnDisConnect() override {
            if (m_cbDisConnected)m_cbDisConnected();
        }

        void OnRecv(const char *data, int32_t size) override;

    private:
        bool m_waitHeader;
        uint32_t m_waitSize;
        MsgTopic::Header m_header;
        std::string m_data;
        std::shared_ptr<std::string> m_reg;
        Callback m_cbConnected;
        Callback m_cbDisConnected;
        RecvMsgCallback m_cbRecvMsg;
    };

    /*!
     * @brief 同步阻塞订阅
     */
    class MsgTopicSubscriberSync {
    public:

        /*!
         * @brief 启动时携带的配置参数
         */
        struct Config {
            bool sendByPack; //!< true代表根据包的数量判断，false代表根据字节数判断
            int32_t maxSend; //!< TopicServer针对此订阅端缓存的最大数据量，单位由‘sendByPack’决定，-1表示无限制
            std::vector<uint32_t> topicArray; //!<需要订阅的topic数组
        };

        MsgTopicSubscriberSync() {}

        /*!
         * @brief 连接TopicServer
         * @param cfg [in] 启动参数
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE Connect(const Config &cfg, int32_t timeoutMs);

        /*!
         * @brief 获取订阅的消息
         * @param topic [in] 消息的topic
         * @param data [in] topic携带的数据
         * @param timeoutMs [in] 超时时间
         * @return ErrCodeE
         */
        ErrCodeE GetMsg(uint32_t &topic, std::string &data, int32_t timeoutMs);

        /*!
         * @brief 断开与TopicServer的链接
         */
        void DisConnect();

        /*!
         * @brief 获取本地地址
         * @return 存储本地地址的字符串
         */
        const char *GetSockName() { return m_path; }

    private:
        SocketUnStream m_sock;
        char m_path[EMX_MAX_PATH_SIZE];
    };
}

#endif //EMX_MSGTOPIC_HPP
