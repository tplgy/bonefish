#ifndef BONEFISH_SERIALIZATION_MSGPACK_SUBSCRIBE_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SUBSCRIBE_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_subscribe_message;

class msgpack_subscribe_message_serializer
{
public:
    wamp_subscribe_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_subscribe_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_SUBSCRIBE_MESSAGE_SERIALIZER_HPP
