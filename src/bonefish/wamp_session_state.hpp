#ifndef BONEFISH_WAMP_SESSION_STATE_HPP
#define BONEFISH_WAMP_SESSION_STATE_HPP

#include <string>

namespace bonefish {

enum class wamp_session_state : uint8_t {
    NONE,
    OPEN,
    CLOSING,
    CLOSED
};

const char* session_state_to_string(const wamp_session_state& state);

} // namespace bonefish

#endif // BONEFISH_WAMP_SESSION_STATE_HPP
