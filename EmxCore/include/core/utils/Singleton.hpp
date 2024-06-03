/**
 * @file Singleton.hpp
 * @author hening
 * @brief 单例模板类
 * @date 2024-5-14
 */

#ifndef EMX_Singleton_HPP
#define EMX_Singleton_HPP

#include <iostream>
#include <memory>
#include <mutex>

namespace Emx {

    template <typename T>
    class Singleton {
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        static T& Get() {
            if (m_instance == nullptr) {
                std::lock_guard<std::mutex> lock(m_locker);
                if (m_instance == nullptr) {
                    m_instance.reset(new T());
                }
            }
            return *m_instance;
        }

        static void Release() {
            std::lock_guard<std::mutex> lock(m_locker);
            m_instance.reset(nullptr);
        }

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;

    private:
        static std::mutex m_locker;
        static std::unique_ptr<T> m_instance;
    };

    template <typename T>
    std::mutex Singleton<T>::m_locker;

    template <typename T>
    std::unique_ptr<T> Singleton<T>::m_instance = nullptr;
}

#endif //EMX_Singleton_HPP
