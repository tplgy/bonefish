#ifndef BONEFISH_SESSION_WAMP_SESSION_STATE_HPP
#define BONEFISH_SESSION_WAMP_SESSION_STATE_HPP

#include <cstdint>
#include <ostream>
#include <string>

namespace bonefish {

enum class wamp_session_state : uint8_t {
    NONE,
    OPEN,
    CLOSING,
    CLOSED
};

const char* session_state_to_string(const wamp_session_state& state);

inline std::ostream& operator<<(std::ostream& os, const wamp_session_state& state)
{
    os << bonefish::session_state_to_string(state);
    return os;
}

} // namespace bonefish

#endif // BONEFISH_SESSION_WAMP_SESSION_STATE_HPP
