//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Shm_HPP
#define EMX_Shm_HPP

#include <cstdint>

namespace Emx {

    //!用于共享内存
    class Shm {
    public:
        Shm() {
            m_shmId = -1;
            m_shmPtr = nullptr;
        }

        ~Shm() {
            Destroy();
        }

        /*!
         * @brief 创建或获取共享内存
         * @param key [in] 共享内存使用的key
         * @param size [in] 共享内存大小
         * @return 共享内存指针
         */
        void *Create(int key, int32_t size);

        /*!
         * @brief 获取共享内存
         * @param key [in] 共享内存使用的key
         * @return 共享内存指针
         */
        void *Create(int key);

        //!断开共享内存
        void Destroy();

        //!获取共享内存大小
        int32_t GetSize();

    private:
        int m_shmId;
        void *m_shmPtr;
    };
}

#endif //EMX_Shm_HPP
