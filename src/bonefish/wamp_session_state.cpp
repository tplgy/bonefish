#include <bonefish/wamp_session_state.hpp>

namespace bonefish {

const char* session_state_to_string(const wamp_session_state& state)
{
    const char* str = nullptr;
    switch (state)
    {
        case wamp_session_state::NONE:
            str = "none";
            break;
        case wamp_session_state::OPEN:
            str = "open";
            break;
        case wamp_session_state::CLOSING:
            str = "closing";
            break;
        case wamp_session_state::CLOSED:
            str = "closed";
            break;
        default:
            str = "<<invalid>>";
            break;
    }

    return str;
}

} // namespace bonefish
