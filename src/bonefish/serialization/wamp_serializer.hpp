#ifndef BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP

#include <bonefish/serialization/serializer_type.hpp>
#include <cstddef>

namespace bonefish
{

class wamp_message;

class wamp_serializer
{
public:
    wamp_serializer();
    virtual ~wamp_serializer();

    virtual serializer_type get_type() const = 0;
    virtual wamp_message* deserialize(const char* buffer, size_t length) const = 0;
    virtual size_t serialize(const wamp_message* message, char* buffer, size_t length) const = 0;
};

inline wamp_serializer::wamp_serializer()
{
}

inline wamp_serializer::~wamp_serializer()
{
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZER_HPP
