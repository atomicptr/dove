#pragma once

#include <algorithm>
#include <functional>
#include <queue>
#include <source_location>
#include <unordered_map>
#include <unordered_set>

#ifdef DOVE_DEBUG
    #include <format>
#endif

#ifndef DOVE_DEBUG_PRINT_FUNC
    #include <print>
    #define DOVE_DEBUG_PRINT_FUNC std::println
#endif

#ifndef DOVE_WARN_PRINT_FUNC
    #include <print>
    #define DOVE_WARN_PRINT_FUNC std::println
#endif

#ifndef DOVE_ERR_PRINT_FUNC
    #include <print>
    #define DOVE_ERR_PRINT_FUNC(...) std::println(stderr, __VA_ARGS__)
#endif

#ifndef DOVE_ASSERT_FUNC
    #include <format>
    #define DOVE_ASSERT_FUNC(condition, ...)                                 \
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
#endif

namespace dove {
    template <typename MessageType, typename MessageData>
    class Broker {
      public:
        using WhoPtr = void*;
        using ListenerFn = std::function<bool(WhoPtr, MessageType, MessageData)>;
        using Listener = std::tuple<WhoPtr, ListenerFn, const std::source_location>;
        using Message = std::tuple<MessageType, MessageData>;

        void add_listener(
            MessageType type,
            WhoPtr who,
            ListenerFn func,
            const std::source_location loc = std::source_location::current()
        ) {
            listeners[type].emplace_back(who, func, loc);

#ifdef DOVE_DEBUG
            DOVE_DEBUG_PRINT_FUNC("Dove: Registered message {} listener at {}", type, loc);
#endif
        }

        void add_listeners(
            std::unordered_set<MessageType> types,
            WhoPtr who,
            ListenerFn func,
            const std::source_location loc = std::source_location::current()
        ) {
            for (auto type : types) {
                add_listener(type, who, func, loc);
            }
        }

        void remove_listener(WhoPtr who) {
            for (auto message_type : listeners) {
                listeners[message_type].erase(std::remove_if(
                    listeners[message_type].begin(),
                    listeners[message_type].end(),
                    [&who](auto& listener) { return listener.get(0) == who; }
                ));
            }
        }

        void process_messages() {
            while (messages.size() > 0) {
                const auto [message_type, message_data] = messages.front();
                messages.pop();

                if (!listeners.contains(message_type)) {
                    continue;
                }

                for (auto [who, func, loc] : listeners[message_type]) {
                    auto processed = func(who, message_type, message_data);

#ifdef DOVE_STRICT_MODE
                    DOVE_ASSERT_FUNC(
                        processed, "Dove: Listener ({}) has not processed registered message: {}", loc, message_type
                    );
#elif defined(DOVE_DEBUG)

                    if (!processed) {
                        DOVE_WARN_PRINT_FUNC(
                            "Dove: Listener ({}) has not processed registered message: {}", loc, message_type
                        );
                    }
#endif
                }
            }
        }

        void post(
            MessageType type,
            MessageData data = {},
            const std::source_location loc = std::source_location::current()
        ) {
            messages.push({type, data});

#ifdef DOVE_DEBUG
            DOVE_DEBUG_PRINT_FUNC("Dove: Received message {} from: {} with data: {}", type, loc, data);
#endif
        }

      private:
        std::unordered_map<MessageType, std::vector<Listener>> listeners;
        std::queue<Message> messages;
    };
}

#if defined(DOVE_DEBUG) && !defined(DOVE_DISABLE_SOURCE_LOCATION_FORMATTER)
template <>
struct std::formatter<std::source_location> : std::formatter<std::string> {
    template <class FormatContext>
    auto format(const std::source_location loc, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "file: {} ({})", loc.file_name(), loc.line());
    }
};
#endif
