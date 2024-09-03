#include "test_base.hpp"

enum class Key {
    A,
    B,
    C
};

struct Test {
    int* a;
    int* b;
    int* c;
};

int main() {
    dove::Broker<Key, Test> b;

    int a_send = 0;
    int b_send = 0;
    int c_send = 0;

    auto func = [](auto _receiver, auto msg, auto data) {
        switch (msg) {
        case Key::A:
            (*data.a) += 1;
            return true;
        case Key::B:
            (*data.b) += 1;
            return true;
        case Key::C:
            (*data.c) += 1;
            return true;
        }

        return false;
    };

    b.add_listeners({Key::A, Key::B, Key::C}, nullptr, func);

    auto t = Test{&a_send, &b_send, &c_send};

    b.post(Key::A, t);
    b.post(Key::A, t);
    b.post(Key::B, t);

    b.process_messages();

    assert(*t.a == 2, "a should be 2");
    assert(*t.b == 1, "b should be 1");
    assert(*t.c == 0, "c should be 0");

    return 0;
}
