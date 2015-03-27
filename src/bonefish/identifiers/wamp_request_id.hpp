#ifndef BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_HPP

#include <bonefish/identifiers/wamp_sequential_id.hpp>

#include <functional>

namespace bonefish {

class wamp_request_id : public wamp_sequential_id
{
public:
    wamp_request_id();
    explicit wamp_request_id(uint64_t id);
};

inline wamp_request_id::wamp_request_id()
    : wamp_sequential_id()
{
}

inline wamp_request_id::wamp_request_id(uint64_t id)
    : wamp_sequential_id(id)
{
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_request_id>
{
    size_t operator()(const bonefish::wamp_request_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_REQUEST_ID_HPP
