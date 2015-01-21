#ifndef BONEFISH_SERIALIZATION_MSGPACK_HELLO_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_HELLO_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_hello_message;

class msgpack_hello_message_serializer
{
public:
    wamp_hello_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_hello_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_HELLO_MESSAGE_SERIALIZER_HPP
