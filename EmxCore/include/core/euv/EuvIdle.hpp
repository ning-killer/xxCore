//
// Created by xiong on 2021/7/12.
//

#ifndef EMX_EuvIdle_HPP
#define EMX_EuvIdle_HPP

#include "EuvLoop.hpp"

namespace Emx {

    //!@brief 空闲回调，当loop处于空闲状态时会触发此事件
    class EuvIdle {
    public:

        //!@brief 当空闲事件触发后调用的回调函数
        using Callback = std::function<void()>;

        EuvIdle() : m_idle(nullptr) {}

        virtual ~EuvIdle() {
            Destroy();
        }

        /*!
         * @brief 创建空闲事件监测
         * @param loop [in] 需要绑定到loop
         * @param cb [in] loop空闲后触发的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop, EuvIdle::Callback cb);

        //!@brief 销毁空闲事件监测
        void Destroy();

    private:
        static void OnDestroy(uv_handle_t *handle);

    private:
        uv_idle_t *m_idle;
        Callback m_cb;
    };
}
#endif //EMX_EuvIdle_HPP
