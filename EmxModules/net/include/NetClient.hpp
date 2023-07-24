//
// Created by xiong on 2021/3/9.
//

#ifndef Emx_NETCLIENT_HPP
#define Emx_NETCLIENT_HPP

#include "EmxCore.hpp"
#include "Net.hpp"

namespace Emx {
    //!用于监听网络连接事件
    class NetClientEvent {
    public:

        NetClientEvent() {
            m_inited = false;
        }

        //!@brief 连接到CoreServer时的回调
        using OnConnected = std::function<void()>;

        /**
         * @brief 创建监听
         * @param loop [in] 需要绑定的loop
         * @param buffer [in] 提供用于缓存的buffer
         * @param size [in] 用于缓存buffer的大小
         * @param cbLink [in] 当连接状态发生改变时的回调函数
         * @param cbConnected [in] 当此对象连接到CoreServer后的回调函数
         */
        void Create(EuvLoop &loop, char *buffer, int size,
                    Net::LinkCallback cbLink, OnConnected cbConnected = nullptr);

        //!@brief 销毁监听
        void Destroy();

    private:
        void OnMsgRecv(uint32_t topic, const char *data, int32_t size);

    private:
        bool m_inited;
        Net::LinkCallback m_cbLink;
        MsgTopicSubscriberAsync m_sub;
    };

    //!@brief 主网卡相关的操作，可以将某个网卡设备配置为主设备
    class NetMainDev {
    public:
        /**
         * @brief 配置主设备类型
         * @param dev [in] 主设备类型
         * @return ErrCodeE
         */
        static ErrCodeE Set(Net::DevE dev);

        /**
         * @brief 获取主设备类型
         * @param dev [out] 主设备类型
         * @return ErrCodeE
         */
        static ErrCodeE Get(Net::DevE &dev);

        /**
         * @brief 设备类型是否支持
         * @param dev [in] 设备类型
         * @return bool
         */
        static bool IsSupported(Net::DevE dev);
    };

    //!@brief 设备操作网卡的客户端接口
    class NetClient : public Net {
    public:
        NetClient(Net::DevE dev) : m_dev(dev) {
            memset(m_interface, 0, InterfaceSize);
        }

        const char *GetInterface() override;

        ErrCodeE SetAddr(Addr &addr) override;

        ErrCodeE GetAddr(Addr &addr) override;

        ErrCodeE SetEna(bool ena) override;

        ErrCodeE GetEna(bool &ena) override;

        ErrCodeE GetLinkStat(LinkStat &link) override;

        ErrCodeE ReLink() override;

    protected:
        char m_interface[InterfaceSize];
        Net::DevE m_dev;
    };

    //!@brief 设备操作Wifi网卡的客户端接口
    class NetClientWifi : public Wifi, public NetClient {
    public:
        NetClientWifi() : NetClient(Net::DevE::Wifi) {}

        ErrCodeE SwitchToMode(ModeE mode) override;

        ErrCodeE SetSTA(const char *ssid, const char *psk) override;

        ErrCodeE SetAP(const char *ssid, const char *psk) override;

        ErrCodeE GetAPList(ApInfoListPtr &list) override;

        ErrCodeE GetInfo(Info &info) override;
    };

    class NetClientDns : public NetDns {
    public:
        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;
    };
}
#endif //Emx_NETCLIENT_HPP
