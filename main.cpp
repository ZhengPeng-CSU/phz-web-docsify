#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

using namespace std::literals;

class A;
class B {
public:
    B() {
        std::cout << "construct B" << std::endl;
    }
    ~B() {
        std::cout << "destruct B" << std::endl;
    }
    void Func(std::weak_ptr<A> weak) {
        if (weak.expired()) {
            std::cout << "nullptr" << std::endl;
        }
        auto ptr = weak.lock();
        std::this_thread::sleep_for(2s);
    }
};

class A : public std::enable_shared_from_this<A> {
public:
    A() {
        std::cout << "construct A" << std::endl;
    }
    ~A() {
        std::cout << "destruct A" << std::endl;
        if(trd_.joinable()) {
            trd_.join();
        }
    }
    void TestFunc() {
        trd_ = std::thread([this]() {
            ThreadFunc();
        });
    }

    void ThreadFunc() {
        std::cout << "thread func " << std::endl;
        b.Func(shared_from_this());
        std::this_thread::sleep_for(3s);
    }

private:
    B b;
    std::thread trd_;
};

int main() {
    {
        auto a_ptr = std::make_shared<A>();
        a_ptr->TestFunc();
        std::this_thread::sleep_for(1s);
    }

    std::this_thread::sleep_for(10s);
}