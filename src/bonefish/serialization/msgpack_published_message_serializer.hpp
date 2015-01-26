#ifndef BONEFISH_SERIALIZATION_MSGPACK_PUBLISHED_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_PUBLISHED_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_published_message;

class msgpack_published_message_serializer
{
public:
    wamp_published_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_published_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_PUBLISHED_MESSAGE_SERIALIZER_HPP
