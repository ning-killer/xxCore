#include "EmxCore.hpp"

using namespace Emx;

class MySingletonClass : public Singleton<MySingletonClass> {
    friend class Singleton<MySingletonClass>;

private:
    MySingletonClass() {
        emxlogd("MySingletonClass Constructor\n");
    }

public:
    void DoSomething() {
        emxlogd("MySingletonClass DoSomething\n");
    }

    ~MySingletonClass() { }
};

int main() {
    MySingletonClass::Get().DoSomething();
    MySingletonClass::Release();
    return 0;
}