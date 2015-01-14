#ifndef BONEFISH_MESSAGE_CODE_HPP
#define BONEFISH_MESSAGE_CODE_HPP

#include <cstdint>

namespace bonefish {

enum class message_code : unsigned
{
    Hello = 1,
    Welcome = 2,
    Abort = 3,
    Challenge = 4,
    Authenticate = 5,
    Goodbye = 6,
    Heartbeat = 7,
    Error = 8,
    Publish = 16,
    Published = 17,
    Subscribe = 32,
    Subscribed = 33,
    Unsubscribe = 34,
    Unsubscribed = 35,
    Event = 36,
    Call = 48,
    Cancel = 49,
    Result = 50,
    Register = 64,
    Registered = 65,
    Unregister = 66,
    Unregistered = 67,
    Invocation = 68,
    Interrupt = 69,
    Yield = 70
};

const char* to_string(message_code code);

} // namespace bonefish

#endif // BONEFISH_MESSAGE_CODE_HPP
