/**
 * @file SafetyQueue.hpp
 * @author hening
 * @brief 多线程环境处理生产者-消费者模型的安全队列类
 * @date 2024-6-17
 */
#ifndef EMX_SafetyQueue_HPP
#define EMX_SafetyQueue_HPP

#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class SafetyQueue {
    public:
        SafetyQueue(int total = 3);
        ~SafetyQueue();

        /*!
         * @brief put数据到队列
         * @param data [in] 数据模板 
         * @return void
         * @note 等待队列非空的时候，通知唤醒解锁等待get操作。
         */
        void Put(const T& data);

        /*!
         * @brief 从队列中get数据
         * @return void
         * @note 队列为空的时候，阻塞线程等待队列非空状态通知解锁。
         */
        T Get();

        /*!
         * @brief 获取队列长度 
         * @return 队列长度
         */
        int Size();

        /*!
         * @brief 清除队列 
         * @return void
         */
        void Clear();

        /*!
         * @brief 设置队列长度上限，默认为3 
         * @param total [in] 配置队列长度
         * @return void
         */
        void SetTotal(int total);

    private:
        int m_total;
        std::mutex m_mtx;
        std::list<T> m_queue;
        std::list<std::pair<std::shared_ptr<bool>, std::shared_ptr<std::condition_variable>>> m_waitQueue;
};
#endif //EMX_SafetyQueue_HPP