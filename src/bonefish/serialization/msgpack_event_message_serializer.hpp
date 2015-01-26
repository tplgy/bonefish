#ifndef BONEFISH_SERIALIZATION_MSGPACK_EVENT_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_EVENT_MESSAGE_SERIALIZER_HPP

#include <cstddef>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_event_message;

class msgpack_event_message_serializer
{
public:
    wamp_event_message* deserialize(const std::vector<msgpack::object>& fields);
    size_t serialize(const wamp_event_message* message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_EVENT_MESSAGE_SERIALIZER_HPP
