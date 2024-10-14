#include "EmxCore.hpp"

using namespace Emx;


/**************************************************************
* 虚继承
* 在 DerivedC 类的 display() 函数中，可以直接访问 baseValue，
* 因为在虚继承的情况下，编译器会确保只有一份 Base 类的实例被创建和使用。
**************************************************************/

// 基类
class BaseV {
public:
    int baseValue;
};

// 派生类A，虚继承自Base
class DerivedAV : virtual public BaseV {
public:
    void setAValue(int val) {
        baseValue = val;
    }
};

// 派生类B，虚继承自Base
class DerivedBV : virtual public BaseV {
public:
    void setBValue(int val) {
        baseValue = val;
    }
};

// 最终派生类C，继承自DerivedA和DerivedB
class DerivedCV : public DerivedAV, public DerivedBV {
public:
    void display() const {
        std::cout << "Base value: " << baseValue << std::endl;
        std::cout << "Base value: " << DerivedAV::baseValue << std::endl;
        std::cout << "Base value: " << DerivedBV::baseValue << std::endl;
    }
}; 

/**************************************************************
* 非虚继承
* 为了消除二义性，必须显式地指定是通过 DerivedA 还是 DerivedB 来访问 Base 类的成员变量：
**************************************************************/

// 基类
class Base {
public:
    int baseValue;
};

// 派生类A，虚继承自Base
class DerivedA : public Base {
public:
    void setAValue(int val) {
        baseValue = val;
    }
};

// 派生类B，虚继承自Base
class DerivedB : public Base {
public:
    void setBValue(int val) {
        baseValue = val;
    }
};

// 最终派生类C，继承自DerivedA和DerivedB
class DerivedC : public DerivedA, public DerivedB {
public:
    void display() const {
        std::cout << "Base value: " << DerivedA::baseValue << std::endl;
        std::cout << "Base value: " << DerivedB::baseValue << std::endl;
    }
}; 

int main() {
    // 非虚继承
    printf("非虚继承显示打印如下：\n");
    DerivedC c;
    c.setAValue(10);
    c.setBValue(100);
    c.display();

    // 虚继承
    printf("虚继承显示打印如下：\n");
    DerivedCV cv;
    cv.setAValue(10);
    cv.setBValue(100);
    cv.display();
    return 0;
}