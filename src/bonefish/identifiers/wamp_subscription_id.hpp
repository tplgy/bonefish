#ifndef BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_HPP

#include <bonefish/identifiers/wamp_sequential_id.hpp>

#include <functional>

namespace bonefish {

class wamp_subscription_id : public wamp_sequential_id
{
public:
    wamp_subscription_id();
    explicit wamp_subscription_id(uint64_t id);
};

inline wamp_subscription_id::wamp_subscription_id()
    : wamp_sequential_id()
{
}

inline wamp_subscription_id::wamp_subscription_id(uint64_t id)
    : wamp_sequential_id(id)
{
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_subscription_id>
{
    size_t operator()(const bonefish::wamp_subscription_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_SUBSCRIPTION_ID_HPP
