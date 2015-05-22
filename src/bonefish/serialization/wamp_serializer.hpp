#ifndef BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP

#include <bonefish/serialization/expandable_buffer.hpp>
#include <bonefish/serialization/wamp_serializer_type.hpp>

#include <cstddef>
#include <msgpack.hpp>

namespace bonefish
{

class wamp_message;

class wamp_serializer
{
public:
    wamp_serializer();
    virtual ~wamp_serializer();

    virtual wamp_serializer_type get_type() const = 0;
    virtual wamp_message* deserialize(const char* buffer, size_t length) const = 0;
    virtual expandable_buffer serialize(const wamp_message* message) const = 0;
};

inline wamp_serializer::wamp_serializer()
{
}

inline wamp_serializer::~wamp_serializer()
{
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP
