#ifndef BONEFISH_SERIALIZATION_MSGPACK_UNSUBSCRIBED_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_UNSUBSCRIBED_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_unsubscribed_message;

class msgpack_unsubscribed_message_serializer
{
public:
    wamp_unsubscribed_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_unsubscribed_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_UNSUBSCRIBED_MESSAGE_SERIALIZER_HPP
