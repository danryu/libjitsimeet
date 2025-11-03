#pragma once
#include <optional>
#include <span>
#include <thread>

// #include <nice/agent.h>
#include <nice.h>

#include "../jingle/jingle.hpp"
#include "../macros/autoptr.hpp"
#include "../xmpp/extdisco.hpp"

namespace ice {
declare_autoptr(GMainLoop, GMainLoop, g_main_loop_unref);
declare_autoptr(NiceAgent, NiceAgent, g_object_unref);
declare_autoptr(GChar, gchar, g_free);

struct MainloopWithRunner {
    AutoGMainLoop mainloop;
    std::thread   runner;

    auto start_runner() -> void;

    static auto create() -> MainloopWithRunner*;

    ~MainloopWithRunner();
};

using AutoMainloop = std::unique_ptr<MainloopWithRunner>;

struct Agent {
    AutoMainloop  mainloop;
    AutoNiceAgent agent;
    guint         stream_id;
    guint         component_id;
};

auto setup(std::span<const xmpp::Service> external_services, const jingle::IceUdpTransport* transport) -> std::optional<Agent>;

auto str_to_sockaddr(const char* addr, uint16_t port) -> NiceAddress;
auto sockaddr_to_str(const NiceAddress& addr) -> std::string;
auto sockaddr_to_port(const NiceAddress& addr) -> uint16_t;
auto candidate_type_to_nice(jingle::CandidateType type) -> std::optional<NiceCandidateType>;
auto candidate_type_from_nice(NiceCandidateType type) -> std::optional<jingle::CandidateType>;

struct LocalCredential {
    AutoGChar ufrag;
    AutoGChar pwd;
};

auto get_local_credentials(const Agent& agent) -> std::optional<LocalCredential>;

struct NiceCandidates {
    GSList*                     list;
    std::vector<NiceCandidate*> candidates;

    ~NiceCandidates();
};

auto get_local_candidates(const Agent& agent) -> NiceCandidates;
} // namespace ice
