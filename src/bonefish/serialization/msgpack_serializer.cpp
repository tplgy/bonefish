#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_event_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_published_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribed_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/serialization/msgpack_abort_message_serializer.hpp>
#include <bonefish/serialization/msgpack_event_message_serializer.hpp>
#include <bonefish/serialization/msgpack_goodbye_message_serializer.hpp>
#include <bonefish/serialization/msgpack_hello_message_serializer.hpp>
#include <bonefish/serialization/msgpack_publish_message_serializer.hpp>
#include <bonefish/serialization/msgpack_published_message_serializer.hpp>
#include <bonefish/serialization/msgpack_subscribe_message_serializer.hpp>
#include <bonefish/serialization/msgpack_subscribed_message_serializer.hpp>
#include <bonefish/serialization/msgpack_unsubscribe_message_serializer.hpp>
#include <bonefish/serialization/msgpack_unsubscribed_message_serializer.hpp>
#include <bonefish/serialization/msgpack_welcome_message_serializer.hpp>
#include <iostream>
#include <msgpack.hpp>
#include <sstream>
#include <stdexcept>

namespace bonefish {

wamp_message* msgpack_serializer::deserialize(const char* buffer, size_t length) const
{
    try {
        // TODO: The extra copy here is lame. Find a more efficient way to
        //       just work on the buffer directly.
        msgpack::unpacked item;
        msgpack::unpacker unpacker;
        unpacker.reserve_buffer(length);
        memcpy(unpacker.buffer(), buffer, length);
        unpacker.buffer_consumed(length);

        std::vector<msgpack::object> fields;
        unpacker.next(&item);
        item.get().convert(&fields);

        wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
        switch (type)
        {
            case wamp_message_type::GOODBYE:
                {
                    msgpack_goodbye_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            case wamp_message_type::HELLO:
                {
                    msgpack_hello_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            case wamp_message_type::PUBLISH:
                {
                    msgpack_publish_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            case wamp_message_type::SUBSCRIBE:
                {
                    msgpack_subscribe_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            case wamp_message_type::UNSUBSCRIBE:
                {
                    msgpack_unsubscribe_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            default:
                break;
        }

        std::stringstream sstr;
        sstr << "no deserializer defined for message " << message_type_to_string(type);
        throw(std::logic_error(sstr.str()));
    } catch (const msgpack::unpack_error& e) {
        std::cerr << "failed to unpack message: " << e.what();
    } catch (const msgpack::type_error& e) {
        std::cerr << "failed to parse message: " << e.what();
    } catch (const std::exception& e) {
        std::cerr << "unhandled exception: " << e.what() << std::endl;
    }

    return nullptr;
}

size_t msgpack_serializer::serialize(const wamp_message* message, char* buffer, size_t length) const
{
    switch (message->get_type())
    {
        case wamp_message_type::ABORT:
            {
                msgpack_abort_message_serializer message_serializer;
                const wamp_abort_message* abort_message =
                        static_cast<const wamp_abort_message*>(message);
                return message_serializer.serialize(abort_message, buffer, length);
            }
            break;
        case wamp_message_type::EVENT:
            {
                msgpack_event_message_serializer message_serializer;
                const wamp_event_message* event_message =
                        static_cast<const wamp_event_message*>(message);
                return message_serializer.serialize(event_message, buffer, length);
            }
            break;
        case wamp_message_type::GOODBYE:
            {
                msgpack_goodbye_message_serializer message_serializer;
                const wamp_goodbye_message* goodbye_message =
                        static_cast<const wamp_goodbye_message*>(message);
                return message_serializer.serialize(goodbye_message, buffer, length);
            }
            break;
        case wamp_message_type::PUBLISHED:
            {
                msgpack_published_message_serializer message_serializer;
                const wamp_published_message* published_message =
                        static_cast<const wamp_published_message*>(message);
                return message_serializer.serialize(published_message, buffer, length);
            }
            break;
        case wamp_message_type::SUBSCRIBED:
            {
                msgpack_subscribed_message_serializer message_serializer;
                const wamp_subscribed_message* subscribed_message =
                        static_cast<const wamp_subscribed_message*>(message);
                return message_serializer.serialize(subscribed_message, buffer, length);
            }
            break;
        case wamp_message_type::UNSUBSCRIBED:
            {
                msgpack_unsubscribed_message_serializer message_serializer;
                const wamp_unsubscribed_message* unsubscribed_message =
                        static_cast<const wamp_unsubscribed_message*>(message);
                return message_serializer.serialize(unsubscribed_message, buffer, length);
            }
            break;
        case wamp_message_type::WELCOME:
            {
                msgpack_welcome_message_serializer message_serializer;
                const wamp_welcome_message* welcome_message =
                        static_cast<const wamp_welcome_message*>(message);
                return message_serializer.serialize(welcome_message, buffer, length);
            }
            break;
        default:
            break;
    }

    std::stringstream sstr;
    sstr << "no serializer defined for message " << message_type_to_string(message->get_type());
    throw(std::logic_error(sstr.str()));
}

} // namespace bonefish
