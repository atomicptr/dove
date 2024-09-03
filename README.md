# dove

A tiny single fileh eader only messaging system for games written in C++23

Inspired by [pidgeon](https://github.com/atomicptr/pidgeon).

## Usage

Dove utilizes a broker to whom you send pre defined messages with data, which you put into a queue and then process them all at once.

First we need to define out message and data types:

```cpp
#pragma once

// This file could be src/game/broker.hpp or something
// Generally we want this at a place where it can actually be
// included and reused

#include <dove/dove.hpp>

// In this example we will update our UI via messages, so when these values
// changei we will send out a message
enum class Message {
    PlayerHPUpdated,
    PlayerManaUpdated,
};

// for this example we will only pass new values so a number
// is sufficient, normally you'd probably use a union or a std::variant here
using MessageData = unsigned int;

// its highly recommended to define a Broker type like this:
using MyBroker = dove::Broker<Message, MessageData>;

// also we only want one global broker in this case:
MyBroker* broker = nullptr;
````

Next we need to initialize the broker somewhere, for this example we just do it in main:

```cpp
int main() {
    broker = new MyBroker;

    // do stuff

    delete broker;
}
```

Now at various locations in the code we can post messages:

```cpp
#include "broker.hpp"

// ...

void game::Player::cast_spell(const Spell& spell) {
    mana_amount -= spell.cost;

    broker->post(Message::PlayerManaUpdated, mana_amount);

    spawn_spell(spell, position, direction);
}
```

And lastly we need to register to receive messages, which we can do like this:

```cpp
#include "broker.hpp"

game::UI::UI() {
    // since we use a class here we need to wrap the on message handler so that we can just handle
    // events in a class context
    auto fn = [this](MyBroker::WhoPtr _receiver, Message message_type, MessageData message_data) {
        return this->on_message(message_type, message_data);
    };

    // now we register the listeners
    broker->add_listener(Message::PlayerHPUpdated, this, fn);
    broker->add_listener(Message::PlayerManaUpdated, this, fn);
}

// we need to return if we handled the message or not, this helps figuring out
// if you registered to something without handling it
bool game::UI::on_message(Message message_type, MessageData message_data) {
    switch (message_type) {
    case Message::PlayerHPUpdated:
        set_hp(message_data);
        return true;
    case Message::PlayerManaUpdated:
        set_mana(message_data);
        return true;
    }

    // in thsi case we haven't handled anything so we return false
    return false;
}
```

And thats how you use dove.

## Configuration

Dove has a few configuration options exposed via defines.

Since C++ still does not offer proper reflection (and we can't print arbitrary things), you will probably have to implement custom formatters for your Message and MessageData types

### DOVE_DEBUG

Enables dove to print debugging information like messages registered, messages posted, etc.

```cpp
// enable like this before you import dove
#define DOVE_DEBUG
```

### DOVE_DEBUG_PRINT_FUNC

The function called to be when debug prints happen, defaults to std::println

```cpp
#define DOVE_DEBUG_PRINT_FUNC std::println
````

### DOVE_WARN_PRINT_FUNC

The function called to be when warning prints happen, defaults to std::println

```cpp
#define DOVE_WARN_PRINT_FUNC std::println
````

### DOVE_ERR_PRINT_FUNC

The function called to be when error prints happen, defaults to std::println (to stderr)

```cpp
#define DOVE_ERR_PRINT_FUNC std::println
````

### DOVE_ASSERT_FUNC

The function/macro called for asserts, defaults to slightly modified [rapture](https://github.com/atomicptr/rapture) assert

```cpp
#define DOVE_ASSERT(condition, ...)                                      \
    if (!(condition)) {                                                  \
        auto rapture_loc = std::source_location::current();              \
        DOVE_ERR_PRINT_FUNC("");                                         \
        DOVE_ERR_PRINT_FUNC(                                             \
            "========= ASSERTATION FAILED {}:{}:{} =========",           \
            rapture_loc.file_name(),                                     \
            rapture_loc.line(),                                          \
            rapture_loc.column()                                         \
        );                                                               \
        DOVE_ERR_PRINT_FUNC("\tAssert :\t{}", (#condition));             \
        DOVE_ERR_PRINT_FUNC("\tMessage:\t{}", std::format(__VA_ARGS__)); \
        DOVE_ERR_PRINT_FUNC("");                                         \
        std::abort();                                                    \
    }
```

### DOVE_DISABLE_SOURCE_LOCATION_FORMATTER

By default dove registers a std::formatter for std::source_location, but you might not want this (or a custom one) so define this to disable it

### License

BSD 0-Clause
