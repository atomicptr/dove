#include "dove.hpp"
#include "rapture.hpp"
#include "test_base.hpp"

enum class Msg {
    Call,
};

using MsgData = int;

struct Handler {
    std::size_t id;
    std::string name;
    bool was_called;
};

int main() {
    std::array<Handler, 4> handlers {};

    handlers.at(0) = {.id = 0, .name = "Andy", .was_called = false};
    handlers.at(1) = {.id = 1, .name = "Beatrice", .was_called = false};
    handlers.at(2) = {.id = 2, .name = "Charles", .was_called = false};
    handlers.at(3) = {.id = 3, .name = "Dieter", .was_called = false};

    using Broker = dove::Broker<Msg, MsgData, std::size_t>;

    auto b = Broker {};

    const auto fn = [&handlers](Broker::WhoPtr who, Msg msg, MsgData data) {
        handlers.at(who).was_called = true;
        return true;
    };

    b.add_listener(Msg::Call, 0, fn);
    b.add_listener(Msg::Call, 2, fn);

    b.post(Msg::Call, 1);

    rpt_expect(handlers.at(0).was_called == false);
    rpt_expect(handlers.at(1).was_called == false);
    rpt_expect(handlers.at(2).was_called == false);
    rpt_expect(handlers.at(3).was_called == false);

    b.process_messages();

    rpt_expect(handlers.at(0).was_called == true);
    rpt_expect(handlers.at(1).was_called == false);
    rpt_expect(handlers.at(2).was_called == true);
    rpt_expect(handlers.at(3).was_called == false);

    handlers.at(2).was_called = false;

    b.remove_listener(2);

    b.post(Msg::Call, 1);
    b.process_messages();

    rpt_expect(handlers.at(0).was_called == true);
    rpt_expect(handlers.at(1).was_called == false);
    rpt_expect(handlers.at(2).was_called == false);
    rpt_expect(handlers.at(3).was_called == false);

    return 0;
}
