#ifndef BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP

#include <bonefish/serialization/serializer.hpp>
#include <cstdint>
#include <memory>

namespace bonefish {

class msgpack_serializer : public serializer
{
public:
    msgpack_serializer();
    virtual ~msgpack_serializer();

    virtual std::unique_ptr<wamp_message> deserialize(const char* buffer, size_t length);
    virtual size_t serialize(const std::unique_ptr<wamp_message>& message, char* buffer, size_t length);
};

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_SERIALIZER_HPP
