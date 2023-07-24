//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Sem_HPP
#define EMX_Sem_HPP

#include "core/EmxTypeDef.hpp"
#include <semaphore.h>

namespace Emx {

    //!进程间同步信号量
    class Sem {
    public:
        Sem() {
            m_semId = nullptr;
        }

        ~Sem() {
            Destroy();
        }

        /*!
         * @brief 创建信号量
         * @param path [in] 信号量绑定地址
         * @param initValue [in] 初始值
         * @return ErrCodeE
         */
        ErrCodeE Create(const char *path, int32_t initValue);

//        ErrCodeE Create(int32_t initValue, int32_t shared);

        /*!
         * @brief 等待信号量
         * @param timeoutMs [in]超时时间，默认永久等待
         * @return ErrCodeE
         */
        ErrCodeE Wait(int32_t timeoutMs = -1);

        //!释放信号量
        ErrCodeE Post();

        //!获取信号量的值
        int32_t Get();

        //!销毁信号量
        void Destroy();

    private:
        sem_t *m_semId;
        char m_name[EMX_MAX_PATH_SIZE];

    };

}

#endif //EMX_Sem_HPP
