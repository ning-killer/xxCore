//
// Created by xiong on 2021/4/8.
//

#ifndef EMX_EuvWork_HPP
#define EMX_EuvWork_HPP

#include "EuvLoop.hpp"

namespace Emx {

    /**!
     * @brief 提供在EuvLoop内调用线程池执行阻塞耗时任务的机制
     */
    class EuvWork {
    public:
        EuvWork() :
                m_loop(nullptr),
                m_work(nullptr),
                m_arg(nullptr),
                m_runCb(nullptr),
                m_afterCb(nullptr),
                m_isWorking(false) {}


        /*!
         * @brief 注册执行耗时阻塞任务的回调函数，此函数运行在loop外的线程池中
         * @param arg [in] 用户自定义的参数指针
         */
        using RunWorkCallBack = std::function<void(void *arg)>;

        /*!
         * @brief 任务执行完毕的回调函数，此函数运行在loop中，在此回调中可以再次Run新的任务
         * @param e [in] 错误信息
         * @param arg [in] 用户自定义的参数指针
         */
        using AfterWorkCallBack = std::function<void(Emx::ErrCodeE e, void *arg)>;


        /*!
         * @brief 创建work，可指定后续运行参数
         * @param loop [in] 需要绑定的loop
         * @param arg [in] 用户自定义的参数，会被传入到runCb与afterCb中
         * @param runCb [in] 执行任务的函数
         * @param afterCb [in] 任务结束后的回调函数
         * @return ErrCodeE
         */
        ErrCodeE Create(EuvLoop *loop,
                        void *arg = nullptr,
                        RunWorkCallBack runCb = nullptr,
                        AfterWorkCallBack afterCb = nullptr);

        /**!
         * @brief 销毁work，如果任务未执行则立刻返回，如果任务在执行中则以阻塞方式等待任务执行完毕
         * @note 此操作可能会阻塞所在loop，不建议在调用频繁的场景中使用
         * @note 此操作执行时，若Run还没有收到afterCb回调(包括未执行和正在执行)，
         * 则此操作会取消任务或等待任务结束并主动调用afterCb回调（参数ErrCodeE == DESTROYED）
         */
        void Destroy();

        /*!
         * @brief 运行任务
         * @param arg [in] 用户自定义的参数，会被传入到runCb与afterCb中
         * @param runCb [in] 执行任务的函数
         * @param afterCb [in] 任务结束后的回调函数
         * @return ErrCodeE
         * @note 注意每个EuvWork在执行此调用之后都需要等待afterCb被调用或者Destroy-Create被调用之后才能Run新的任务
         * @note 如果Create创建时已经指定了参数，那么可以使用创建时的参数或是修改某些参数，nullptr表示不修改参数
         */
        ErrCodeE Run(void *arg = nullptr,
                     RunWorkCallBack runCb = nullptr,
                     AfterWorkCallBack afterCb = nullptr);

        /*!
         * @brief 获取work是否正在运行
         * @return 是否正在运行
         */
        bool IsWorking() { return m_isWorking; }

    private:
        static void OnDestroy(uv_work_t *req, int status);

    private:
        EuvLoop *m_loop;
        uv_work_t *m_work;
        void *m_arg;
        RunWorkCallBack m_runCb;
        AfterWorkCallBack m_afterCb;
        bool m_isWorking;
    };

}

#endif //EMX_EuvWork_HPP
