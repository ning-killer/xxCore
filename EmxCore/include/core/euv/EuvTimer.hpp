//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVTIMER_HPP
#define EMX_EUVTIMER_HPP

#include "EuvLoop.hpp"

namespace Emx {

    //!@brief 基于EuvLoop的定时器，很有用的一个东西
    class EuvTimer {
    public:

        //!@brief 超时回调
        using Callback = std::function<void()>;

        EuvTimer() : m_timer(nullptr) {}

        virtual ~EuvTimer() {
            Destroy();
        }

        /*!
         * @brief 创建定时器，并绑定loop
         * @param loop [in] 需要绑定的loop
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop &loop);

        //!@brief 销毁定时器
        void Destroy();

        /*!
         * @brief 启动定时器，超时后会触发回调
         * @param timeout [in] 第一次超时时间
         * @param repeat [in] 第一次超时之后后面每次超时的时间间隔，0表示只超时一次
         * @param cb [in] 超时后触发的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Start(uint64_t timeout, uint64_t repeat, Callback cb);

        //!@brief 停止定时器
        void Stop() { uv_timer_stop(m_timer); }

//        void Again() { uv_timer_again(m_timer); }
//
//        void SetRepeat(uint64_t repeat) { uv_timer_set_repeat(m_timer, repeat); }
//
//        uint64_t GetRepeat() { return uv_timer_get_repeat(m_timer); }
//
//        uint64_t GetTimeout() { return m_timer->timeout; }
//
//        uint64_t GetStartId() { return m_timer->start_id; }
//
//        uint64_t GetLoopTime() { return m_timer->loop->time; }

    private:
        static void OnDestroy(uv_handle_t *handle);

    private:
        uv_timer_t *m_timer;
        Callback m_cb;
    };
}


#endif //EMX_EUVTIMER_HPP
