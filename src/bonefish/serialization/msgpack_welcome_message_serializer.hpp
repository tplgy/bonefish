#ifndef BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_WELCOME_MESSAGE_SERIALIZER_HPP

#include <cstdint>
#include <memory>
#include <msgpack.hpp>

namespace bonefish {

class wamp_message;

class msgpack_welcome_message_serializer
{
public:
    std::unique_ptr<wamp_message> deserialize(const std::vector<msgpack::object>& fields);
    virtual size_t serialize(const std::unique_ptr<wamp_message>& message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WELCOME_MESSAGE_SERIALIZER_HPP
