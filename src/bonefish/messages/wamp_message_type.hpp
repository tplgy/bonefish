#ifndef BONEFISH_MESSAGES_WAMP_MESSAGE_TYPE_HPP
#define BONEFISH_MESSAGES_WAMP_MESSAGE_TYPE_HPP

#include <cstdint>

namespace bonefish {

enum class wamp_message_type : unsigned
{
    HELLO = 1,
    WELCOME = 2,
    ABORT = 3,
    CHALLENGE = 4,
    AUTHENTICATE = 5,
    GOODBYE = 6,
    HEARTBEAT = 7,
    ERROR = 8,
    PUBLISH = 16,
    PUBLISHED = 17,
    SUBSCRIBE = 32,
    SUBSCRIBED = 33,
    UNSUBSCRIBE = 34,
    UNSUBSCRIBED = 35,
    EVENT = 36,
    CALL = 48,
    CANCEL = 49,
    RESULT = 50,
    REGISTER = 64,
    REGISTERED = 65,
    UNREGISTER = 66,
    UNREGISTERED = 67,
    INVOCATION = 68,
    INTERRUPT = 69,
    YIELD = 70
};

const char* message_type_to_string(const wamp_message_type& type);

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_MESSAGE_TYPE_HPP
