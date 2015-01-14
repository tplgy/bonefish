#include <bonefish/message_code.hpp>
#include <cassert>

namespace bonefish {

const char* to_string(message_code code)
{
    const char* str = nullptr;

    switch(code)
    {
        case message_code::Hello:
            str = "hello";
            break;
        case message_code::Welcome:
            str = "welcome";
            break;
        case message_code::Abort:
            str = "abort";
            break;
        case message_code::Challenge:
            str = "challenge";
            break;
        case message_code::Authenticate:
            str = "authenticate";
            break;
        case message_code::Goodbye:
            str = "goodbye";
            break;
        case message_code::Heartbeat:
            str = "heartbeat";
            break;
        case message_code::Error:
            str = "error";
            break;
        case message_code::Publish:
            str = "publish";
            break;
        case message_code::Published:
            str = "published";
            break;
        case message_code::Subscribe:
            str = "subscribe";
            break;
        case message_code::Subscribed:
            str = "subscribed";
            break;
        case message_code::Unsubscribe:
            str = "unsubscribe";
            break;
        case message_code::Unsubscribed:
            str = "unsubscribed";
            break;
        case message_code::Event:
            str = "event";
            break;
        case message_code::Call:
            str = "call";
            break;
        case message_code::Cancel:
            str = "cancel";
            break;
        case message_code::Result:
            str = "result";
            break;
        case message_code::Register:
            str = "register";
            break;
        case message_code::Registered:
            str = "registered";
            break;
        case message_code::Unregister:
            str = "unregister";
            break;
        case message_code::Unregistered:
            str = "unregistered";
            break;
        case message_code::Invocation:
            str = "invocation";
            break;
        case message_code::Interrupt:
            str = "interrupt";
            break;
        case message_code::Yield:
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
