#pragma once
#include <string_view>

#include "jingle/jingle.hpp"
#include "ws/client.hpp"

namespace colibri {
struct Colibri {
    ws::client::Context ws_context;

    auto set_last_n(int n) -> void;
    auto set_default_max_height(int max_height) -> void;
    auto set_source_max_height(std::string_view source_name, int max_height) -> void;

    ~Colibri();

    static auto connect(const jingle::Jingle& initiate_jingle, bool secure) -> std::unique_ptr<Colibri>;
};
} // namespace colibri
