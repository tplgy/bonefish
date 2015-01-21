#ifndef BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class welcome_message;

class msgpack_welcome_message_serializer
{
public:
    welcome_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const welcome_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WELCOME_MESSAGE_SERIALIZER_HPP
