#ifndef BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP

#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializer_type.hpp>
#include <cstddef>

namespace bonefish {

class wamp_message;

class msgpack_serializer : public wamp_serializer
{
public:
    msgpack_serializer();
    virtual ~msgpack_serializer() override;

    virtual wamp_serializer_type get_type() const override;
    virtual wamp_message* deserialize(const char* buffer, size_t length) const override;
    virtual size_t serialize(const wamp_message* message, char* buffer, size_t length) const override;
};

inline msgpack_serializer::msgpack_serializer()
{
}


inline msgpack_serializer::~msgpack_serializer()
{
}

inline wamp_serializer_type msgpack_serializer::get_type() const
{
    return wamp_serializer_type::MSGPACK;
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_SERIALIZER_HPP
