#ifndef BONEFISH_SERIALIZATION_MSGPACK_SUBSCRIBED_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SUBSCRIBED_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_subscribed_message;

class msgpack_subscribed_message_serializer
{
public:
    wamp_subscribed_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_subscribed_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_SUBSCRIBED_MESSAGE_SERIALIZER_HPP
