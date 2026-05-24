#include <cstdint>
#include <string>

#if !defined _WIN32
#include <netdb.h>
#else
#include <WinSock2.h>
#endif

// NOTE: keep `const char*` (no top-level `const`) to match the hostaddr.hpp
// declaration -- MSVC encodes a parameter's top-level const into the decorated name,
// so `const char* const` here would fail to link (LNK2019).
auto hostname_to_addr(const char* hostname) -> std::string {
    auto r = std::string();

    const auto host = gethostbyname(hostname);
    if(host == NULL || host->h_length != 4 || host->h_addrtype != AF_INET) {
        return r;
    }
    for(auto i = 0; host->h_addr_list[i]; i += 1) {
        auto addr = (uint8_t*)host->h_addr_list[i];
        for(auto i = 0; i < 4; i += 1) {
            r += std::to_string(addr[i]);
            r += ".";
        }
        r.pop_back();
    }
    return r;
}
