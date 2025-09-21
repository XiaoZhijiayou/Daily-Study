#include <iostream>
using namespace std;

// 基类
class Base {
public:
    virtual void print() {   // 虚函数
        cout << "Base::print()" << endl;
    }
};

// 派生类
class Derived : public Base {
public:
    void print() override {  // 重写虚函数
        cout << "Derived::print()" << endl;
    }
};

int main() {
    Base* b = new Base();       // 基类对象
    Base* d = new Derived();    // 基类指针指向派生类对象

    b->print();   // 调用 Base::print()
    d->print();   // 运行时多态，调用 Derived::print()

    delete b;
    delete d;

    return 0;
}
