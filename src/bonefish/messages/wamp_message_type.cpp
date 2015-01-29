#include <bonefish/messages/wamp_message_type.hpp>
#include <cassert>

namespace bonefish {

const char* message_type_to_string(wamp_message_type type)
{
    const char* str = nullptr;

    switch(type)
    {
        case wamp_message_type::HELLO:
            str = "hello";
            break;
        case wamp_message_type::WELCOME:
            str = "welcome";
            break;
        case wamp_message_type::ABORT:
            str = "abort";
            break;
        case wamp_message_type::CHALLENGE:
            str = "challenge";
            break;
        case wamp_message_type::AUTHENTICATE:
            str = "authenticate";
            break;
        case wamp_message_type::GOODBYE:
            str = "goodbye";
            break;
        case wamp_message_type::HEARTBEAT:
            str = "heartbeat";
            break;
        case wamp_message_type::ERROR:
            str = "error";
            break;
        case wamp_message_type::PUBLISH:
            str = "publish";
            break;
        case wamp_message_type::PUBLISHED:
            str = "published";
            break;
        case wamp_message_type::SUBSCRIBE:
            str = "subscribe";
            break;
        case wamp_message_type::SUBSCRIBED:
            str = "subscribed";
            break;
        case wamp_message_type::UNSUBSCRIBE:
            str = "unsubscribe";
            break;
        case wamp_message_type::UNSUBSCRIBED:
            str = "unsubscribed";
            break;
        case wamp_message_type::EVENT:
            str = "event";
            break;
        case wamp_message_type::CALL:
            str = "call";
            break;
        case wamp_message_type::CANCEL:
            str = "cancel";
            break;
        case wamp_message_type::RESULT:
            str = "result";
            break;
        case wamp_message_type::REGISTER:
            str = "register";
            break;
        case wamp_message_type::REGISTERED:
            str = "registered";
            break;
        case wamp_message_type::UNREGISTER:
            str = "unregister";
            break;
        case wamp_message_type::UNREGISTERED:
            str = "unregistered";
            break;
        case wamp_message_type::INVOCATION:
            str = "invocation";
            break;
        case wamp_message_type::INTERRUPT:
            str = "interrupt";
            break;
        case wamp_message_type::YIELD:
            str = "yield";
            break;
        default:
            assert(0);
            str = "<<invalid>>";
            break;
    }

    return str;
}

} // namespace bonefish
