#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/serialization/msgpack_hello_message_serializer.hpp>
#include <bonefish/serialization/msgpack_welcome_message_serializer.hpp>
#include <bonefish/messages/message_type.hpp>
#include <bonefish/messages/hello_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/welcome_message.hpp>
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

        message_type type = static_cast<message_type>(fields[0].as<unsigned>());
        switch (type)
        {
            case message_type::Hello:
                {
                    msgpack_hello_message_serializer message_serializer;
                    wamp_message* message = message_serializer.deserialize(fields);
                    return message;
                }
                break;
            case message_type::Welcome:
                {
                    msgpack_welcome_message_serializer message_serializer;
                    return message_serializer.deserialize(fields);
                }
                break;
            default:
                break;
        }

        std::stringstream sstr;
        sstr << "no serializer defined for message " << to_string(type);
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
        case message_type::Hello:
            {
                msgpack_hello_message_serializer message_serializer;
                const hello_message* hello = static_cast<const hello_message*>(message);
                return message_serializer.serialize(hello, buffer, length);
            }
            break;
        case message_type::Welcome:
            {
                msgpack_welcome_message_serializer message_serializer;
                const welcome_message* welcome = static_cast<const welcome_message*>(message);
                return message_serializer.serialize(welcome, buffer, length);
            }
            break;
        default:
            break;
    }

    std::stringstream sstr;
    sstr << "no serializer defined for message " << to_string(message->get_type());
    throw(std::logic_error(sstr.str()));
}

} // namespace bonefish
