//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVASYNC_HPP
#define EMX_EUVASYNC_HPP

#include <mutex>
#include "EuvLoop.hpp"

namespace Emx {

    //!@brief 实现基于Loop的异步事件通知
    class EuvAsync {
    public:

        //!@brief 事件到达时触发的回调函数
        using Callback = std::function<void()>;

        EuvAsync() : m_async(nullptr) {}

        virtual ~EuvAsync() {
            Destroy();
        }

        /*!
         * @brief 创建异步通知
         * @param loop [in] 需要绑定的loop
         * @param cb [in] 注册一个回调函数，当时间触发时执行
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop, Callback cb);

        //!@brief 销毁异步通知
        void Destroy();

        /*!
         * @brief 触发一次异步通知，当事件回调函数执行完毕之前，多次的触发会被合并为一次，
         *        此函数可以在Loop之外执行
         * @return ErrCodeE
         */
        ErrCodeE Send();

    private:
        static void OnDestroy(uv_handle_t *handle);

    private:
        uv_async_t *m_async;
        std::mutex m_mutex;
        Callback m_cb;
    };
}


#endif //EMX_EUVASYNC_HPP
