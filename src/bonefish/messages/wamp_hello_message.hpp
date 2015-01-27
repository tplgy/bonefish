#ifndef BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/utility/wamp_uri.hpp>
#include <unordered_set>

namespace bonefish {

//
// [HELLO, Realm|uri, Details|dict]
//
class wamp_hello_message : public wamp_message
{
public:
    wamp_hello_message();
    virtual ~wamp_hello_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_uri& get_realm() const;
    const std::unordered_set<wamp_role>& get_roles() const;

    void set_realm(const wamp_uri& realm);
    bool add_role(const wamp_role& role);

private:
    wamp_uri m_realm;
    std::unordered_set<wamp_role> m_roles;
};

inline wamp_hello_message::wamp_hello_message()
    : m_realm()
    , m_roles()
{
}

inline wamp_hello_message::~wamp_hello_message()
{
}

inline wamp_message_type wamp_hello_message::get_type() const
{
    return wamp_message_type::HELLO;
}

inline const wamp_uri& wamp_hello_message::get_realm() const
{
    return m_realm;
}

inline const std::unordered_set<wamp_role>& wamp_hello_message::get_roles() const
{
    return m_roles;
}

inline void wamp_hello_message::set_realm(const wamp_uri& realm)
{
    m_realm = realm;
}

inline bool wamp_hello_message::add_role(const wamp_role& role)
{
    auto result = m_roles.insert(role);
    return result.second;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP
