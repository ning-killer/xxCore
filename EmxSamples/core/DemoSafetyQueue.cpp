#include "EmxCore.hpp"

using namespace Emx;

struct DataItem {
    int size;
    uint8_t *data = nullptr;
};

SafetyQueue<DataItem*> g_queue(10);

void Produce() {
    DataItem *data = new DataItem();
    data->size = 52;
    data->data = (uint8_t*)malloc(data->size);
    emxlogi("put data[%p, %d]\n", data->data, data->size);
    g_queue.Put(data);
}

void ConsumeB() {
    EuvLoop loopSubB;
    EuvTimer bTimer;
    loopSubB.Init("loopSubB", nullptr);
    bTimer.Create(loopSubB);
    loopSubB.Start(true);
    bTimer.Start(0,2000,[](){
        DataItem *data = g_queue.Get();
        emxlogi("current size[%d]; get data[%p, %d]\n", g_queue.Size(), data->data, data->size);
    });
}

void ConsumeC() {
    EuvLoop loopSubC;
    EuvTimer cTimer;
    loopSubC.Init("loopSubC", nullptr);
    cTimer.Create(loopSubC);
    loopSubC.Start(true);
    cTimer.Start(0,2000,[](){
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