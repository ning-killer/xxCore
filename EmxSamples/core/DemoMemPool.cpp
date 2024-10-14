#include "EmxCore.hpp"

using namespace Emx;

class MyData {
    public:
        MyData(): m_data(nullptr), m_size(1024) {
            if (m_data == nullptr) {
                m_data = (uint8_t*)malloc(m_size);
            }
        }

        ~MyData() {
            if (m_data != nullptr) {
                free(m_data);
                m_data = nullptr;
            }
        }

    private:
        uint8_t *m_data;
        uint32_t m_size;
};


int main() {
    MemPool<MyData> myDataPool(1024);
    for (int i = 0; i < 1024; i++) {
        printf("my pool [%d] data [%p]\n", i, myDataPool.Malloc());
    }
    printf("more than data [%p]\n", myDataPool.Malloc());
    return 0;
}