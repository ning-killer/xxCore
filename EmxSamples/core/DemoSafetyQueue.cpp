#include "EmxCore.hpp"

using namespace Emx;

struct DataItem {
    int size;
    uint8_t *data = nullptr;
};

DataItem* CloneCb(const DataItem* data) {
    if (data == nullptr) {
        return nullptr;
    }
    DataItem* cloned = new DataItem();
    cloned->size = data->size;
    cloned->data = (uint8_t*)malloc(cloned->size);
    if (cloned->data != nullptr) {
        memcpy(cloned->data, data->data, cloned->size);
    }
    return cloned;
}

void FreeCb(const DataItem* data) {
    if (data != nullptr) {
        if (data->data != nullptr) {
            free(data->data);
        }
        delete data;
        data = nullptr;
    }
}

SafetyQueue<DataItem*> g_queue(10, CloneCb, FreeCb);

void Produce() {
    DataItem *data = new DataItem();
    data->size = 52;
    data->data = (uint8_t*)malloc(data->size);
    emxlogi("put data[%p, %d]\n", data->data, data->size);
    g_queue.Put(data);
}

EuvLoop g_loopSubB;
EuvTimer g_bTimer;
void ConsumeB() {
    g_loopSubB.Init("loopSubB", nullptr);
    g_bTimer.Create(g_loopSubB);
    g_loopSubB.Start(true);
    g_bTimer.Start(0,2000,[](){
        DataItem *data = g_queue.Get();
        emxlogi("current size[%d]; get data[%p, %d]\n", g_queue.Size(), data->data, data->size);
    });
}

EuvLoop g_loopSubC;
EuvTimer g_cTimer;
void ConsumeC() {
    g_loopSubC.Init("loopSubC", nullptr);
    g_cTimer.Create(g_loopSubC);
    g_loopSubC.Start(true);
    g_cTimer.Start(0,2000,[](){
        DataItem *data = g_queue.Get();
        emxlogi("current size[%d]; get data[%p, %d]\n", g_queue.Size(), data->data, data->size);
    });
}


int main(int argc, char *argv[]) {
    EuvLoop loopMainA;
    EuvTimer aTimer;
    loopMainA.Init("loopMainA", nullptr);
    // loopMainA进行生产
    aTimer.Create(loopMainA);
    aTimer.Start(0,3000,[](){
        Produce();
    });

    // 模拟多线程进行消费
    ConsumeB();
    ConsumeC();
    loopMainA.Start(false);
}