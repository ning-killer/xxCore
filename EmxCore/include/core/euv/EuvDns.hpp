//
// Created by xiong on 2021/6/29.
//

#ifndef EMX_EuvDns_HPP
#define EMX_EuvDns_HPP

#include "uv.h"
#include <functional>
#include "core/EmxTypeDef.hpp"

namespace Emx {

    //!@brief 用于异步解析DNS
    class EuvDns {
    public:
        EuvDns() :
                m_loop(nullptr),
                m_resolver(nullptr),
                m_isRunning(false),
                m_cb(nullptr) {}

        //!@brief DNS解析类型
        enum class TypeE : uint8_t {
            Ipv4,//!<只解析IPV4类型地址
            Ipv6,//!<只解析IPV6类型地址
            UnSpec//!<IPV4与IPV6类型地址均可
        };
        /*!
         * @brief 获取到解析结果后触发的回调函数
         * @param e [in] 解析结果是否正确
         * @param addr [in] 解析到的地址
         * @param arg [in] 调用run函数的时候传入的用户自定义私有数据指针
         */
        using OnResolved = std::function<void(Emx::ErrCodeE e, addrinfo *addr, void *arg)>;

        /*!
         * @brief 创建DNS解析
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop *loop);

        /**!
         * @brief 销毁DNS解析，如果任务未执行则立刻返回，如果任务在执行中则以阻塞方式等待任务执行完毕
         * @note 此操作可能会阻塞所在loop，不建议在调用频繁的场景中使用
         * @note 此操作执行时，若Run还没有收到OnResolved回调(包括未执行和正在执行)，
         * 则此操作会取消任务或等待任务结束并主动调用OnResolved回调（参数ErrCodeE == DESTROYED）
         */
        void Destroy();

        /*!
         * @brief 开始运行地址解析
         * @param server [in] 需要解析的url
         * @param cb [in] 注册一个解析完成后的回调函数
         * @param arg [in] 用户自定义私有数据指针
         * @param type [in] 解析类型
         * @return  ErrCodeE
         */
        ErrCodeE Run(const char *server, OnResolved cb,
                     void *arg = nullptr, TypeE type = TypeE::UnSpec);

    private:
        static void OnDestroy(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res);

    private:
        EuvLoop *m_loop;
        struct Data {
            void *obj;
            void *arg;
        } m_data;
        uv_getaddrinfo_t *m_resolver;
        bool m_isRunning;
        OnResolved m_cb;
        char m_server[EMX_MAX_PATH_SIZE];
    };
}

#endif //EMX_EuvDns_HPP
