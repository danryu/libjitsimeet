#include "colibri.hpp"
#include "macros/logger.hpp"
#include "uri.hpp"

#define CUTIL_MACROS_PRINT_FUNC(...) LOG_ERROR(logger, __VA_ARGS__)
#include "macros/unwrap.hpp"

namespace colibri {
namespace {
auto logger = Logger("colibri");

auto find_transport(const jingle::Jingle& jingle) -> const jingle::IceUdpTransport* {
    for(const auto& c : jingle.content) {
        if(!c.transport.empty()) {
            return &c.transport[0];
        }
    }
    return nullptr;
}
} // namespace

auto Colibri::set_last_n(const int n) -> void {
    const auto payload = std::format(R"({{"colibriClass":"ReceiverVideoConstraints","lastN":{}}})", n);
    LOG_INFO(logger, "Colibri send: {}", payload);
    ensure(ws_context.send(payload));
}

auto Colibri::set_default_max_height(const int max_height) -> void {
    const auto payload = std::format(R"({{"colibriClass":"ReceiverVideoConstraints","defaultConstraints":{{"maxHeight":{}}}}})", max_height);
    LOG_INFO(logger, "Colibri send: {}", payload);
    ensure(ws_context.send(payload));
}

Colibri::~Colibri() {
}

auto Colibri::connect(const jingle::Jingle& initiate_jingle, const bool secure) -> std::unique_ptr<Colibri> {
    unwrap(transport, find_transport(initiate_jingle));
    ensure(!transport.websocket.empty());
    const auto& url = transport.websocket[0].url;
    unwrap(ws_uri, URI::parse(url));
    LOG_INFO(logger, "connecting to colibri at {}", url);

    const auto uri_domain = std::string(ws_uri.domain);
    const auto uri_path   = std::string(ws_uri.path);
    auto       obj        = std::unique_ptr<Colibri>(new Colibri());
    ensure(obj->ws_context.init({
        .address   = uri_domain.data(),
        .path      = uri_path.data(),
        .protocol  = "xmpp",
        .port      = ws_uri.port,
        .ssl_level = secure ? ws::client::SSLLevel::Enable : ws::client::SSLLevel::TrustSelfSigned,
    }));
    obj->ws_context.dump_packets = true;
    return obj;
}
} // namespace colibri
