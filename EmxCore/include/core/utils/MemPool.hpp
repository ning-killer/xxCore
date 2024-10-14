/**
 * @file MemPool.hpp
 * @author hening
 * @brief 内存池模型(防止内存碎片，提供内存访问效率，主要面向针对内存访问高性能要求场景)
 * @date 2024-10-9
 */
#ifndef EMX_MEMPOOL_HPP
#define EMX_MEMPOOL_HPP

#include <cstddef>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <cassert>

template<typename T>
class MemPool {
    public:
        explicit MemPool(size_t blockSize = 1024) 
        : m_freeList(nullptr) 
        , m_blockSize(blockSize){    
            AddBlock(); 
        }

        ~MemPool() {    
            for (auto block : m_blocks) {        
                ::operator delete(block); 
            }
        }

        MemPool(const MemPool&) = delete;    
        MemPool& operator=(const MemPool&) = delete;

        T* Malloc()  { 
            std::lock_guard<std::mutex> lock(m_mutex); 
            if (!m_freeList) {
                AddBlock();
            }
            FreeNode* node = m_freeList;
            m_freeList = node->next;
            return reinterpret_cast<T*>(node);
        }

        void Free(T* ptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
            node->next = m_freeList;
            m_freeList = node;
        }

    private:
        void AddBlock() {
            size_t size = sizeof(FreeNode) > sizeof(T) ? sizeof(FreeNode) : sizeof(T);
            char* block = static_cast<char*>(::operator new(size * m_blockSize));
            m_blocks.push_back(block);
            for (size_t i = 0; i < m_blockSize; ++i) {
                FreeNode* node = reinterpret_cast<FreeNode*>(block + i * size);
                node->next = m_freeList;
                m_freeList = node;
            }
        }

    private:
        struct FreeNode {        
            FreeNode* next = nullptr;    
        };
        FreeNode* m_freeList;
        std::vector<void*> m_blocks;
        size_t m_blockSize;
        std::mutex m_mutex;
};

#endif // EMX_MEMPOOL_HPP