#ifndef BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP
#define BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP

#include <bonefish/roles/wamp_role.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <unordered_set>

namespace bonefish {

class wamp_welcome_details
{
public:
    wamp_welcome_details();
    virtual ~wamp_welcome_details();

    msgpack::object marshal(msgpack::zone* zone=nullptr);
    void unmarshal(const msgpack::object& details);

    const std::unordered_set<wamp_role>& get_roles() const;
    void add_role(const wamp_role& role);

private:
    msgpack::zone m_zone;
    std::unordered_set<wamp_role> m_roles;
};

inline wamp_welcome_details::wamp_welcome_details()
    : m_zone()
{
}

inline wamp_welcome_details::~wamp_welcome_details()
{
}

inline const std::unordered_set<wamp_role>& wamp_welcome_details::get_roles() const
{
    return m_roles;
}

inline void wamp_welcome_details::add_role(const wamp_role& role)
{
    m_roles.insert(role);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP
