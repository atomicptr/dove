#include "test_base.hpp"

#include <map>

using CounterType = unsigned int;

enum class Msg {
    Hello,
    Bye,
};

using MsgData = std::variant<int>;
using Broker = dove::Broker<Msg, MsgData>;
dove::Broker<Msg, MsgData> broker;

class Handler {
public:
    Handler(std::map<Msg, CounterType>& message_counter);

private:
    std::map<Msg, CounterType>& message_counter;

    void register_messages();
    bool on_message(Msg type, MsgData data);
};

Handler::Handler(std::map<Msg, CounterType>& message_counter) : message_counter(message_counter) {
    register_messages();
}

void Handler::register_messages() {
    auto fn = [this](Broker::WhoPtr _receiver, Msg type, MsgData data) {
        return this->on_message(type, data);
    };

    broker.add_listeners({Msg::Hello, Msg::Bye}, this, fn);
}

bool Handler::on_message(Msg type, MsgData data) {
    message_counter.at(type) += 1;
    return true;
}

int main() {
    const CounterType num_handlers = 10000;

    std::map<Msg, CounterType> message_counter {
        {Msg::Hello, 0},
        {Msg::Bye, 0}
    };

    std::vector<Handler> handlers;

    for (auto i = 0; i < num_handlers; i++) {
        handlers.push_back(message_counter);
    }

    rpt_expect(message_counter.at(Msg::Hello) == 0);
    rpt_expect(message_counter.at(Msg::Bye) == 0);

    broker.post(Msg::Hello, 5);

    rpt_expect(message_counter.at(Msg::Hello) == 0);
    rpt_expect(message_counter.at(Msg::Bye) == 0);

    broker.process_messages();

    rpt_expect(message_counter.at(Msg::Hello) == num_handlers);
    rpt_expect(message_counter.at(Msg::Bye) == 0);

    broker.post(Msg::Hello, 5);
    broker.post(Msg::Hello, 5);

    for (auto i = 0; i < num_handlers; i++) {
        broker.post(Msg::Bye, 5);
    }

    // call it many times just because
    broker.process_messages();
    broker.process_messages();
    broker.process_messages();

    rpt_expect(message_counter.at(Msg::Hello) == num_handlers * 3);
    rpt_expect(message_counter.at(Msg::Bye) == num_handlers * num_handlers);

    return 0;
}
