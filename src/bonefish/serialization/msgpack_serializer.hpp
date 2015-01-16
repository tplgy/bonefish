#ifndef BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP

#include <bonefish/serialization/serializer.hpp>
#include <bonefish/serialization/msgpack_hello_message_serializer.hpp>
#include <bonefish/serialization/msgpack_welcome_message_serializer.hpp>
#include <msgpack.hpp>
#include <sstream>
#include <stdexcept>

namespace bonefish {

class msgpack_serializer : public serializer
{
public:
    msgpack_serializer();
    virtual ~msgpack_serializer();

    virtual wamp_message* deserialize(const char* buffer, size_t length);
    virtual size_t serialize(const wamp_message* message, char* buffer, size_t length);
};

inline msgpack_serializer::msgpack_serializer()
{
}

inline msgpack_serializer::~msgpack_serializer()
{
}

inline wamp_message* msgpack_serializer::deserialize(const char* buffer, size_t length)
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
                    return message_serializer.deserialize(buffer, length);
                }
                break;
            case message_type::Welcome:
                {
                    msgpack_welcome_message_serializer message_serializer;
                    return message_serializer.deserialize(buffer, length);
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

inline size_t msgpack_serializer::serialize(const wamp_message* message, char* buffer, size_t length)
{
    switch (message->get_type())
    {
        case message_type::Hello:
            {
                msgpack_hello_message_serializer message_serializer;
                return message_serializer.serialize(message, buffer, length);
            }
            break;
        case message_type::Welcome:
            {
                msgpack_welcome_message_serializer message_serializer;
                return message_serializer.serialize(message, buffer, length);
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

#endif // BONEFISH_SERIALIZATION_SERIALIZER_HPP
