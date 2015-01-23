#ifndef BONEFISH_SERIALIZATION_MSGPACK_GOODBYE_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_GOODBYE_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_goodbye_message;

class msgpack_goodbye_message_serializer
{
public:
    wamp_goodbye_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_goodbye_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_GOODBYE_MESSAGE_SERIALIZER_HPP
