#include <bonefish/messages/wamp_message_factory.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_call_message.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_event_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_invocation_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_published_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_registered_message.hpp>
#include <bonefish/messages/wamp_register_message.hpp>
#include <bonefish/messages/wamp_result_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_unregistered_message.hpp>
#include <bonefish/messages/wamp_unregister_message.hpp>
#include <bonefish/messages/wamp_unsubscribed_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/messages/wamp_yield_message.hpp>

namespace bonefish {
namespace wamp_message_factory {

wamp_message* create_message(wamp_message_type type)
{
    wamp_message* message = nullptr;

    switch(type)
    {
        case wamp_message_type::HELLO:
            message = new wamp_hello_message;
            break;
        case wamp_message_type::WELCOME:
            message = new wamp_welcome_message;
            break;
        case wamp_message_type::ABORT:
            message = new wamp_abort_message;
            break;
        case wamp_message_type::CHALLENGE:
            message = nullptr;
            break;
        case wamp_message_type::AUTHENTICATE:
            message = nullptr;
            break;
        case wamp_message_type::GOODBYE:
            message = new wamp_goodbye_message;
            break;
        case wamp_message_type::HEARTBEAT:
            message = nullptr;
            break;
        case wamp_message_type::ERROR:
            message = new wamp_error_message;
            break;
        case wamp_message_type::PUBLISH:
            message = new wamp_publish_message;
            break;
        case wamp_message_type::PUBLISHED:
            message = new wamp_published_message;
            break;
        case wamp_message_type::SUBSCRIBE:
            message = new wamp_subscribe_message;
            break;
        case wamp_message_type::SUBSCRIBED:
            message = new wamp_subscribed_message;
            break;
        case wamp_message_type::UNSUBSCRIBE:
            message = new wamp_unsubscribe_message;
            break;
        case wamp_message_type::UNSUBSCRIBED:
            message = new wamp_unsubscribed_message;
            break;
        case wamp_message_type::EVENT:
            message = new wamp_event_message;
            break;
        case wamp_message_type::CALL:
            message = new wamp_call_message;
            break;
        case wamp_message_type::CANCEL:
            message = nullptr;
            break;
        case wamp_message_type::RESULT:
            message = new wamp_result_message;
            break;
        case wamp_message_type::REGISTER:
            message = new wamp_register_message;
            break;
        case wamp_message_type::REGISTERED:
            message = new wamp_registered_message;
            break;
        case wamp_message_type::UNREGISTER:
            message = new wamp_unregister_message;
            break;
        case wamp_message_type::UNREGISTERED:
            message = new wamp_unregistered_message;
            break;
        case wamp_message_type::INVOCATION:
            message = new wamp_invocation_message;
            break;
        case wamp_message_type::INTERRUPT:
            message = nullptr;
            break;
        case wamp_message_type::YIELD:
            message = new wamp_yield_message;
            break;
    }

    return message;
}

} // namespace wamp_message_factory
} // namespace bonefish
