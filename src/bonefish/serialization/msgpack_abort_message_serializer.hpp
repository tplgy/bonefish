#ifndef BONEFISH_SERIALIZATION_MSGPACK_ABORT_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_ABORT_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_abort_message;

class msgpack_abort_message_serializer
{
public:
    wamp_abort_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_abort_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_ABORT_MESSAGE_SERIALIZER_HPP
