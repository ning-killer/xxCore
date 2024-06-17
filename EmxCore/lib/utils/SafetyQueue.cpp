#include "core/utils/SafetyQueue.hpp"

template<typename T>
SafetyQueue<T>::SafetyQueue(int total) : m_total(total) {
    
}

template<typename T>
SafetyQueue<T>::~SafetyQueue() {
    std::unique_lock<std::mutex> locker(m_mtx);
    m_queue.clear();
    m_waitQueue.clear();
}

template<typename T>
void SafetyQueue<T>::Put(const T& data) {
    std::unique_lock<std::mutex> locker(m_mtx);
    m_queue.emplace_back(data);

    while ((int)m_queue.size() > m_total) {
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

template<typename T>
T SafetyQueue<T>::Get() {
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

template<typename T>
int SafetyQueue<T>::Size() {
    std::unique_lock<std::mutex> locker(m_mtx);
    return m_queue.size();
}

template<typename T>
void SafetyQueue<T>::Clear() {
    std::unique_lock<std::mutex> locker(m_mtx);
    m_queue.clear();
}

template<typename T>
void SafetyQueue<T>::SetTotal(int total) {
    std::unique_lock<std::mutex> locker(m_mtx);
    m_total = total;
}