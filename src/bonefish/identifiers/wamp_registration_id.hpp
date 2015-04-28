#ifndef BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_HPP

#include <bonefish/identifiers/wamp_sequential_id.hpp>

#include <functional>
#include <ostream>

namespace bonefish {

class wamp_registration_id : public wamp_sequential_id
{
public:
    wamp_registration_id();
    explicit wamp_registration_id(uint64_t id);
};

inline wamp_registration_id::wamp_registration_id()
    : wamp_sequential_id()
{
}

inline wamp_registration_id::wamp_registration_id(uint64_t id)
    : wamp_sequential_id(id)
{
}

inline std::ostream& operator<<(std::ostream& os, const wamp_registration_id& rid)
{
    if (rid.is_valid()) {
        os << rid.id();
    } else {
        os << "<<invalid>>";
    }

    return os;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_registration_id>
{
    size_t operator()(const bonefish::wamp_registration_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_REGISTRATION_ID_HPP
