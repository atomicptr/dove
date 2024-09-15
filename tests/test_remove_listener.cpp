#include "test_base.hpp"

#include <string>

struct Test {
    int* messages_send;
};

class Handler {
  public:
    bool on_message(std::string msg, Test data) {
        assert(msg == "test", "expected message to be 'test'");
        *data.messages_send += 1;
        return true;
    }
};

int main() {
    dove::Broker<std::string, Test> b;

    int messages_send = 0;

    auto make_wrapper = [](Handler* h) {
        return [h](auto receiver, auto msg, auto data) {
            assert(static_cast<const void*>(receiver) == static_cast<const void*>(h), "receiver should stay as is");
            return h->on_message(msg, data);
        };
    };

    Handler* h1 = new Handler;
    Handler* h2 = new Handler;

    b.add_listener("test", h1, make_wrapper(h1));
    b.add_listener("test", h2, make_wrapper(h2));

    b.post("test", {&messages_send});

    b.process_messages();

    assert(messages_send == 2, "expect the message to be sent twice");

    b.remove_listener(h1);

    b.post("test", {&messages_send});

    b.process_messages();

    assert(messages_send == 3, "expect the message to be sent thrice in total");

    delete h1;
    delete h2;

    return 0;
}
