#include <bonefish/messages/message_type.hpp>
#include <cassert>

namespace bonefish {

const char* to_string(const message_type& type)
{
    const char* str = nullptr;

    switch(type)
    {
        case message_type::Hello:
            str = "hello";
            break;
        case message_type::Welcome:
            str = "welcome";
            break;
        case message_type::Abort:
            str = "abort";
            break;
        case message_type::Challenge:
            str = "challenge";
            break;
        case message_type::Authenticate:
            str = "authenticate";
            break;
        case message_type::Goodbye:
            str = "goodbye";
            break;
        case message_type::Heartbeat:
            str = "heartbeat";
            break;
        case message_type::Error:
            str = "error";
            break;
        case message_type::Publish:
            str = "publish";
            break;
        case message_type::Published:
            str = "published";
            break;
        case message_type::Subscribe:
            str = "subscribe";
            break;
        case message_type::Subscribed:
            str = "subscribed";
            break;
        case message_type::Unsubscribe:
            str = "unsubscribe";
            break;
        case message_type::Unsubscribed:
            str = "unsubscribed";
            break;
        case message_type::Event:
            str = "event";
            break;
        case message_type::Call:
            str = "call";
            break;
        case message_type::Cancel:
            str = "cancel";
            break;
        case message_type::Result:
            str = "result";
            break;
        case message_type::Register:
            str = "register";
            break;
        case message_type::Registered:
            str = "registered";
            break;
        case message_type::Unregister:
            str = "unregister";
            break;
        case message_type::Unregistered:
            str = "unregistered";
            break;
        case message_type::Invocation:
            str = "invocation";
            break;
        case message_type::Interrupt:
            str = "interrupt";
            break;
        case message_type::Yield:
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
