#ifndef BONEFISH_SERIALIZATION_MSGPACK_UNSUBSCRIBE_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_UNSUBSCRIBE_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_unsubscribe_message;

class msgpack_unsubscribe_message_serializer
{
public:
    wamp_unsubscribe_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_unsubscribe_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_UNSUBSCRIBE_MESSAGE_SERIALIZER_HPP
