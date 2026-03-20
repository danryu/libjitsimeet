#pragma once
#include <coop/generator.hpp>
#include <coop/single-event.hpp>
#include <coop/task-injector.hpp>

#include "ws/client.hpp"

namespace ws::client {
using OnDataReceivedAsync = coop::Async<void>(PrependableBuffer payload);

struct AsyncContext : Context {
    std::function<OnDataReceivedAsync> handler;
    coop::SingleEvent                  disconnected;

    auto init(coop::TaskInjector& injector, const ContextParams& params) -> bool;
    auto process_until_finish() -> coop::Async<void>;
};
} // namespace ws::client
