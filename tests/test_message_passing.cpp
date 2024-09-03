#include "test_base.hpp"

#include <string>

struct Test {
    bool* message_send;
};

int main() {
    dove::Broker<std::string, Test> b;

    bool message_send = false;

    b.add_listener("test", nullptr, [](auto _receiver, auto msg, auto data) {
        assert(msg == "test", "expected message to be 'test'");
        *data.message_send = true;
        return true;
    });

    b.post("test", {&message_send});

    b.process_messages();

    assert(message_send, "expected message to be send");
    
    return 0;
}
