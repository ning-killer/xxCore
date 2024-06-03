//
// Created by xiong on 2021/3/9.
//

#ifndef EMX_Net_HPP
#define EMX_Net_HPP

#include "EmxCore.hpp"
#include <vector>
#include <cstring>

namespace Emx {
    //!@brief 提供设备网络信息的获取与配置
    class Net {
    public:
        static const int Ipv4AddrSize = 16;//!<IPV4字符串最大大小
        static const int Ipv6AddrSize = 48;//!<IPV6字符串最大大小
        static const int MaxAddrSize = Ipv6AddrSize;//!<IP地址字符串最大大小
        static const int MacAddrSize = 32;//!<MAC字符串最大大小
        static const int InterfaceSize = 12;//!<网卡接口字符串最大大小
        static const int DevSize = 12;//!<设备类型名称字符串最大大小

        //!@brief 网卡设备类型
        enum class DevE : uint8_t {
            None = 0,//!<无设备
            Eth,//!<以太网
            Wifi,//!<Wifi
            Mobile,//!<4G/5G
        };

        //!@brief 网络地址信息
        struct Addr {
            bool dhcp;//!<是否开启dhcp
            char ip4[Ipv4AddrSize];//!<IPV4地址
            char netmask4[Ipv4AddrSize];//!<IPV4子网掩码
            char gateway4[Ipv4AddrSize];//!<IPV4网关
            char ip6[Ipv6AddrSize];//!<IPV6地址
            char netmask6[Ipv6AddrSize];//!<IPV6子网掩码
            char gateway6[Ipv6AddrSize];//!<IPV6网关
            char mac[MacAddrSize];//!<MAC地址
        };

        //!@brief 网络连接状态
        struct LinkStat {
            static const int MsgSize = 256;
            bool phy; //!<phy connected
            bool lan4; //!<got ipv4 addr ,lan connected
            bool lan6; //!<got ipv6 addr ,lan connected
            bool internet4; //!<ipv4 internet accessible
            bool internet6; //!<ipv6 internet accessible
            char msg[MsgSize];//!<附加消息
        };

        //!@brief 网络连接状态消息
        struct LinkMsg {
            Net::DevE dev;//!<网卡设备类型
            LinkStat link;//!<网络连接状态
        };

        /**
         * @brief 连接状态回调
         * @param msg [in] 连接信息
         * @return ErrCodeE
         */
        using LinkCallback = std::function<void(LinkMsg &msg)>;

        /**
         * @brief 获取网卡接口名称(eg.eth0/wlan0)
         * @return 网卡设备名称
         */
        virtual const char *GetInterface() = 0;

        /**
         * @brief 配置地址，字符串内容为空的字段，在配置的时候会被忽略
         * @param addr [in] 地址信息
         * @return ErrCodeE
         */
        virtual ErrCodeE SetAddr(Addr &addr) = 0;

        /**
         * @brief 配置临时地址，字符串内容为空的字段，在配置的时候会被忽略
         * @param addr [in] 地址信息
         * @return ErrCodeE
         */
        virtual ErrCodeE SetTempAddr(Addr &addr) = 0;

        /**
         * @brief 获取当前地址信息
         * @param addr [out] 地址信息
         * @return ErrCodeE
         */
        virtual ErrCodeE GetAddr(Addr &addr) = 0;

        /**
         * @brief 配置网卡使能
         * @param ena [in] 是否使能
         * @return ErrCodeE
         */
        virtual ErrCodeE SetEna(bool ena) = 0;

        /**
         * @brief 获取网卡使能
         * @param ena [out] 是否使能
         * @return ErrCodeE
         */
        virtual ErrCodeE GetEna(bool &ena) = 0;

        /**
         * @brief 获取网卡连接状态
         * @param link [out] 网卡连接状态
         * @return ErrCodeE
         */
        virtual ErrCodeE GetLinkStat(LinkStat &link) = 0;

        /**
         * @brief 断开网络，重新连接
         * @return ErrCodeE
         */
        virtual ErrCodeE ReLink() = 0;

        /**
         * @brief 获取网卡名称字符串(eg.wifi/eth/mobile)
         * @param dev [in] 网卡名称枚举
         * @return 网卡名称字符串
         */
        static const char *GetDevName(DevE dev);

        /**
         * @brief 获取网卡名称枚举
         * @param name [in] 获取网卡名称字符串
         * @return 网卡名称枚举
         */
        static DevE GetDevType(const char *name);

        /**
         * @brief 判断输入字符串是否为ipv4地址类型
         * @param addr [in] 需要检查的字符串
         * @return bool
         */
        static bool IsIpv4Addr(const char *addr);

        /**
         * @brief 判断输入字符串是否为ipv6地址类型
         * @param addr [in] 需要检查的字符串
         * @return bool
         */
        static bool IsIpv6Addr(const char *addr);

        /**
         * @brief 获取本机地址信息
         * @param interface [in] 网卡接口
         * @param addr [out] 本机地址
         */
        static void GetLocalAddr(const char *interface, Addr &addr);
    };

    //!@brief 提供设备Wifi网络信息的获取与配置
    class Wifi {
    public:
        static const int MaxSsidSize = 256;//!<SSID字符串最大大小
        static const int MaxBssidSize = 64;//!<BSSID字符串最大大小
        static const int MaxPskSize = 256;//!<PSK字符串最大大小

        //!Wifi工作模式
        enum class ModeE : uint8_t {
            None = 0,//!<未工作
            AP,//!<AP热点模式
            STA//!<STA模式
        };

        //!加密方式
        enum class KeyMgmtE : uint8_t {
            NONE = 0,
            WEP = 1,
            WPA_PSK = 2,
            WPA_WPA2_PSK = 3,
            WPA2_PSK = 4,
            OTHER = 5
        };

        //!Wifi相关信息
        struct Info {
            ModeE mode;//!<工作模式
            char ssid[MaxSsidSize];//!<SSID
            char bssid[MaxBssidSize];//!<BSSID
            KeyMgmtE mgmt;//!<加密方式
            int signalLevel;//!<信号等级
            int linkSpeed;//!<连接速率
        };

        //!AP热点信息
        struct ApInfo {
            char ssid[MaxSsidSize];//!<SSID
            char bssid[MaxBssidSize];//!<BSSID
            KeyMgmtE mgmt;//!<加密方式
            int signalLevel;//!<信号等级
        };

        /**
         * @brief 切换Wifi工作模式
         * @param mode [in] 工作模式
         * @return ErrCodeE
         */
        virtual ErrCodeE SwitchToMode(ModeE mode) = 0;

        /**
         * @brief 配置STA连接的wifi热点
         * @param ssid [in] 需要连接的AP热点的名称
         * @param psk [in] 需要连接的AP热点的密码
         * @return ErrCodeE
         */
        virtual ErrCodeE SetSTA(const char *ssid, const char *psk) = 0;

        /**
         * @brief 配置自身AP热点名称密码
         * @param ssid [in] 自身AP热点的名称
         * @param psk [in] 自身AP热点的密码
         * @return ErrCodeE
         */
        virtual ErrCodeE SetAP(const char *ssid, const char *psk) = 0;


        using ApInfoList = std::vector<ApInfo>;
        using ApInfoListPtr = std::shared_ptr<ApInfoList>;

        /**
         * @brief 扫描获取周围的AP热点信息
         * @param list [out] 周围的AP热点信息
         * @return ErrCodeE
         */
        virtual ErrCodeE GetAPList(ApInfoListPtr &list) = 0;

        /**
         * @brief 获取当前wifi信息
         * @param info [out] wifi信息
         * @return ErrCodeE
         */
        virtual ErrCodeE GetInfo(Info &info) = 0;
    };

    class NetDns {
    public:
        struct Param {
        public:
            void Serialize(Json::Value &json) const;

            void Parse(const Json::Value &json);

        public:
            bool manual;
            struct Addr {
                char ip[Net::MaxAddrSize];
            };
            std::vector<Addr> server;
            std::vector<Addr> append;
        };

    public:
        virtual ErrCodeE GetParam(Param &param) = 0;

        virtual ErrCodeE SetParam(const Param &param) = 0;
    };
}


#endif //EMX_Net_HPP
