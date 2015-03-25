#ifndef BONEFISH_ROLES_WAMP_ROLE_TYPE_HPP
#define BONEFISH_ROLES_WAMP_ROLE_TYPE_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>

namespace bonefish {

enum class wamp_role_type : uint16_t
{
    CALLEE,
    CALLER,
    PUBLISHER,
    SUBSCRIBER,
    DEALER,
    BROKER
};

const char* role_type_to_string(const wamp_role_type& type);

wamp_role_type role_type_from_string(const std::string& type);

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_role_type>
{
    size_t operator()(const bonefish::wamp_role_type& type) const
    {
        return hash<std::underlying_type<bonefish::wamp_role_type>::type>()(
                static_cast<std::underlying_type<bonefish::wamp_role_type>::type>(type));
    }
};

} // namespace std

#endif // BONEFISH_ROLES_WAMP_ROLE_TYPE_HPP
