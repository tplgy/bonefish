#ifndef BONEFISH_SERIALIZATION_SERIALIZER_TYPE_HPP
#define BONEFISH_SERIALIZATION_SERIALIZER_TYPE_HPP

#include <cstddef>
#include <string>
#include <type_traits>

namespace bonefish {

enum class serializer_type : unsigned
{
    MSGPACK
};

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::serializer_type>
{
    size_t operator()(const bonefish::serializer_type& type) const
    {
        return hash<std::underlying_type<bonefish::serializer_type>::type>()(
                static_cast<std::underlying_type<bonefish::serializer_type>::type>(type));
    }
};

} // namespace std

#endif // BONEFISH_SERIALIZATION_SERIALIZER_TYPE_HPP
