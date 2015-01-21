#ifndef BONEFISH_SERIALIZATION_MSGPACK_HELLO_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_HELLO_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class hello_message;

class msgpack_hello_message_serializer
{
public:
    hello_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const hello_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_HELLO_MESSAGE_SERIALIZER_HPP
