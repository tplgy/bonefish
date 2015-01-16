#ifndef BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP

#include <bonefish/messages/welcome_message.hpp>

namespace bonefish {

class msgpack_welcome_message_serializer
{
public:
    wamp_message* deserialize(const char* buffer, size_t length);
    virtual size_t serialize(const wamp_message* message, char* buffer, size_t length);
};

inline wamp_message* msgpack_welcome_message_serializer::deserialize(const char* buffer, size_t length)
{
    return nullptr;
}

inline size_t msgpack_welcome_message_serializer::serialize(const wamp_message* message, char* buffer, size_t length)
{
    return 0;
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WELCOME_MESSAGE_SERIALIZER_HPP
