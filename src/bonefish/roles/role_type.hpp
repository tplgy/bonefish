#ifndef BONEFISH_ROLES_ROLE_TYPE_HPP
#define BONEFISH_ROLES_ROLE_TYPE_HPP

#include <cstddef>
#include <string>
#include <type_traits>

namespace bonefish {

enum class role_type : uint16_t
{
    callee,
    caller,
    publisher,
    subscriber,
    dealer,
    broker
};

const char* role_type_to_string(const role_type& type);

role_type role_type_from_string(const std::string& type);

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::role_type>
{
    size_t operator()(const bonefish::role_type& type) const
    {
        return hash<std::underlying_type<bonefish::role_type>::type>()(
                static_cast<std::underlying_type<bonefish::role_type>::type>(type));
    }
};

} // namespace std

#endif // BONEFISH_ROLES_ROLE_TYPE_HPP
