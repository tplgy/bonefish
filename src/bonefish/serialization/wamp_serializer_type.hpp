#ifndef BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP
#define BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP

#include <cstddef>
#include <string>
#include <type_traits>

namespace bonefish {

enum class wamp_serializer_type : unsigned
{
    MSGPACK
};

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_serializer_type>
{
    size_t operator()(const bonefish::wamp_serializer_type& type) const
    {
        return hash<std::underlying_type<bonefish::wamp_serializer_type>::type>()(
                static_cast<std::underlying_type<bonefish::wamp_serializer_type>::type>(type));
    }
};

} // namespace std

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP
