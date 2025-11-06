#pragma once
#include "jingle/jingle.hpp"
#include "ws/client.hpp"

namespace colibri {
struct Colibri {
    ws::client::Context ws_context;

    auto set_last_n(int n) -> void;
    auto set_default_max_height(int max_height) -> void;

    ~Colibri();

    static auto connect(const jingle::Jingle& initiate_jingle, bool secure) -> std::unique_ptr<Colibri>;
};
} // namespace colibri
