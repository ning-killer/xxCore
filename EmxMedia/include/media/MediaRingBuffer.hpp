//
// Created by xiong on 2021/7/30.
//

#ifndef EMX_MediaRingBuffer_HPP
#define EMX_MediaRingBuffer_HPP

#include <string>
#include "core/EmxUtils.hpp"

namespace Emx {

    //!@brief 媒体环形共享缓冲区写类(注意：所有操作都是非线程安全的)
    class MediaRingBufferWriter {
    public:
        /**
         * @brief 创建或关联一个缓冲区
         * @param key [in] 缓冲区的识别码
         * @param size [in] 缓冲区的大小
         * @return ErrCodeE
         */
        ErrCodeE Create(int32_t key, int32_t size);

        /**
         * @brief 销毁或断开缓冲区
         */
        void Destroy() { m_shm.Destroy(); }

        /**
         * @brief 发送数据到缓冲区
         * @param data [in] 数据内存地址
         * @param size [in] 数据大小
         * @param magicId [in] 用于判断数据是否被改写，尽量保证每次push都会变化
         * @retval -1:失败
         * @retval other: 写入地址与缓冲区起始地址的的偏移量
         */
        int32_t Push(char *data, int32_t size, uint32_t magicId);

        /**
         * @brief 从缓冲区获取一块内存
         * @param size [in] 期望获取内存大小
         * @param magicId [in] 用于判断数据是否被改写，尽量保证每次Get都会变化
         * @param data [out] 获取到的内存地址
         * @retval -1:失败
         * @retval other: 获取地址与缓冲区起始地址的的偏移量
         */
        int32_t GetFreeBuffer(int32_t size, uint32_t magicId, char **data);

    private:
        Shm m_shm;
        char *m_buffer;
        int32_t m_bufferSize;
        int32_t m_off;
    };

    //!@brief 媒体环形共享缓冲区读类(注意：所有操作都是非线程安全的)
    class MediaRingBufferReader {
    public:
        /**
         * @brief 关联一个缓冲区
         * @param key [in] 缓冲区的识别码
         * @return ErrCodeE
         */
        void Create(int32_t key);

        /**
         * @brief 断开缓冲区
         */
        void Destroy() { m_shm.Destroy(); }

        /**
         * @brief 从缓冲区里读取一块数据
         * @param off [in] 读取地址与缓冲区起始地址的的偏移量
         * @param size [in] 读取大小
         * @param magicId [in] 用于判断数据是否被改写，需要与此数据在Push或GetFreeBuffer时填写的magicId一致
         * @param data [out] 读取到的数据
         * @return ErrCodeE
         */
        ErrCodeE Pop(int32_t off, int32_t size, uint32_t magicId, std::string &data);

    private:
        int32_t m_key;
        Shm m_shm;
        char *m_buffer;
        int32_t m_bufferSize;
    };
}

#endif //EMX_MediaRingBuffer_HPP
