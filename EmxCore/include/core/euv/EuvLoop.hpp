//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_EUVLOOP_HPP
#define EMX_EUVLOOP_HPP

#include <functional>
#include "uv.h"
#include "core/EmxTypeDef.hpp"

namespace Emx {

    //!@brief 实现多路复用循环的主体，所有基于EuvLoop的事件都应绑定到一个EuvLoop上
    class EuvLoop {
    public:

        //!@brief 当循环退出时触发的回调，在此回调中应关闭所有未关闭的绑定到此loop的描述符，否则循环将一直阻塞无法退出
        using Callback = std::function<void()>;

        //!@brief 初始化模式
        enum ModeE {
            Default,//!< 默认loop模式，此模式下会使用libuv为每个进程提供的一个静态loop作为EuvLoop，当一个进程只需要一个loop的时候可使用这个模式
            Dynamic//!< 动态loop模式，此模式下会动态的malloc一个loop
        };

        EuvLoop();

        /*!
         * @brief 初始化一个loop
         * @param name [in] 为这个loop起个响当当的名字，也就是配置loop所依附的线程名字
         * @param onQuit [in] 当执行Stop或者StopAndDeInit时会触发此回调函数，
         * 在此回调中应关闭所有未关闭的绑定到此loop的描述符，否则循环将一直阻塞无法退出
         * @param mode [in] 使用loop的方式，默认为动态模式
         * @return ErrCodeE
         */
        ErrCodeE Init(const char *name, Callback onQuit, ModeE mode = Dynamic);

        /*!
         * @brief 开始loop循环，执行此函数之后loop就真正的运行起来了
         * @param newThread [in] 当此标识为true时会创建一个新的线程来跑loop，此函数不会被阻塞
         * 否则的话就使用当前线程来跑loop，此函数会被阻塞在这里，
         * 当一个进程只需要一个loop的时候可以使用newThread=false的方式
         * @return ErrCodeE
         */
        ErrCodeE Start(bool newThread = true);

        //!@brief loop当前是否处于运行状态
        bool IsRunning() { return uv_loop_alive(m_loop); }


        //!@brief 停止loop
        void Stop();

        //!@brief 去初始化loop，此函数会阻塞至所有描述符关闭并退出loop
        void DeInit();

        /*!
         * @brief 一次性调用Stop和DeInit两个函数，当只有一个loop需要停止时调用此函数，当有多个loop都需要停止时，
         * 可以先调用所有loop的Stop，然后再调用他们的DeInit，这样所有的loop结束的可以快一些
         */
        void StopAndDeInit() {
            Stop();
            DeInit();
        }

        /*!
         * @brief 获取loop名字
         * @return loop名字
         */
        const char *GetName() { return m_name.c_str(); }

        /*!
         * @brief 获取libuv中的loop句柄
         * @return libuv中的loop句柄
         */
        uv_loop_t *GetLoop() { return m_loop; }

        //!@brief 打印绑定到loop中的所有句柄
        void PrintAllHandles() { uv_print_all_handles(m_loop, stderr); }

        //!@brief 打印绑定到loop中的所有正在监听状态的句柄
        void PrintActiveHandles() { uv_print_active_handles(m_loop, stderr); }

        /*!
         * @brief 获取loop是否处于退出状态
         * 当Stop函数被调用后，可能loop无法立刻捕获并执行此事件
         * 如果此时待执行的其他事件较多，可能会引起loop无法及时退出
         * 其他事件的回调可以使用此函数判断当loop处于退出状态时及时退出回调
         * @return libuv中的loop句柄
         */
        bool isQuiting() { return m_quit; }

    private:
        void Close();

    private:
        std::string m_name;
        Callback m_cb;
        uv_async_t m_async;
        uv_thread_t m_thread;
        uv_loop_t *m_loop;
        ModeE m_mode;
        bool m_quit;
    };
}


#endif //EMX_EUVLOOP_HPP
