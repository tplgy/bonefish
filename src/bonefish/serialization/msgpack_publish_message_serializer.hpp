#ifndef BONEFISH_SERIALIZATION_MSGPACK_PUBLISH_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_PUBLISH_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_publish_message;

class msgpack_publish_message_serializer
{
public:
    wamp_publish_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_publish_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_PUBLISH_MESSAGE_SERIALIZER_HPP
