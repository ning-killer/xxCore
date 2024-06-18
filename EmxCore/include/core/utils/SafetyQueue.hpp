/**
 * @file SafetyQueue.hpp
 * @author hening
 * @brief 多线程环境处理生产者-消费者模型的安全队列类
 * @date 2024-6-17
 */
#ifndef EMX_SAFETYQUEUE_HPP
#define EMX_SAFETYQUEUE_HPP

#include <list>
#include <mutex>
#include <condition_variable>
#include <functional>

template<typename T>
class SafetyQueue {
    public:
        /*!
         * @brief 数据克隆
         * @param data [in] 数据深拷贝源数据
         * @return T 数据深拷贝目标数据
         */
        using OnCloned = std::function<T(const T& data)>;

        /*!
         * @brief 数据释放
         * @param data [in] 数据释放数据
         * @return void
         */
        using OnFreed = std::function<void(T& data)>;

        SafetyQueue(int total = 3, 
            OnCloned clonedCb = nullptr, 
            OnFreed freedCb = nullptr) 
            : m_total(total), m_clonedCb(clonedCb), m_freedCb(freedCb) {
        }
        
        ~SafetyQueue() {
            Clear();
        }

        /*!
         * @brief put数据到队列
         * @param data [in] 数据模板 
         * @return void
         * @note 等待队列非空的时候，通知唤醒解锁等待get操作。
         */
        void Put(const T& data) {
            std::unique_lock<std::mutex> locker(m_mtx);
            if (m_clonedCb != nullptr) {
                m_queue.emplace_back(m_clonedCb(data));
            } else {
                m_queue.emplace_back(data);
            }

            while ((int)m_queue.size() > m_total) {
                if (m_freedCb != nullptr) {
                    m_freedCb(m_queue.front());
                }
                m_queue.pop_front();
            }

            // 如果存在等待线程，通知一个线程继续执行
            if (!m_waitQueue.empty()) {
                auto wait = m_waitQueue.front();
                m_waitQueue.pop_front();
                (*(wait.first)) = false;
                wait.second->notify_one();
            }
        }

        /*!
         * @brief 从队列中get数据
         * @return T 数据
         * @note 队列为空的时候，阻塞线程等待队列非空状态通知解锁。
         */
        T Get() {
            std::unique_lock<std::mutex> locker(m_mtx);
            while (m_queue.empty()) {
                // 队列为空的时候，线程阻塞等待队列非空状态唤醒。
                std::shared_ptr<bool> is_empty = std::make_shared<bool>(true);
                std::shared_ptr<std::condition_variable> cv = std::make_shared<std::condition_variable>();
                m_waitQueue.emplace_back(std::make_pair(is_empty, cv));
                cv->wait(locker, [is_empty]() { return !(*is_empty); });
            }
            T data = m_queue.front();
            m_queue.pop_front();
            return data;
        }

        /*!
         * @brief 获取队列长度 
         * @return 队列长度
         */
        int Size() {
            std::unique_lock<std::mutex> locker(m_mtx);
            return m_queue.size();
        }

        /*!
         * @brief 清除队列 
         * @return void
         */
        void Clear() {
            std::unique_lock<std::mutex> locker(m_mtx);
            if (m_freedCb != nullptr) {
                for(auto &data : m_queue) {
                    if (data != nullptr) {
                        m_freedCb(data);
                    }
                }
            }
            m_queue.clear();
            m_waitQueue.clear();
        }

        /*!
         * @brief 设置队列长度上限，默认为3 
         * @param total [in] 配置队列长度
         * @return void
         */
        void SetTotal(int total) {
            std::unique_lock<std::mutex> locker(m_mtx);
            m_total = total;
        }

    private:
        int m_total;
        std::mutex m_mtx;
        std::list<T> m_queue;
        std::list<std::pair<std::shared_ptr<bool>, std::shared_ptr<std::condition_variable>>> m_waitQueue;
        OnCloned m_clonedCb;
        OnFreed m_freedCb;
};

#endif // EMX_SAFETYQUEUE_HPP