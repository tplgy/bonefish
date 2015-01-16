#ifndef BONEFISH_HELLO_MESSAGE_HPP
#define BONEFISH_HELLO_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/roles/role.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace bonefish {

//
// [HELLO, Realm|uri, Details|dict]
//
class hello_message : public wamp_message
{
public:
    hello_message();
    ~hello_message();

    virtual message_type get_type() const;

    const std::string& get_realm() const;
    const std::vector<role>& get_roles() const;

    void set_realm(const std::string& realm);
    void add_role(const role& r);

private:
    std::string m_realm;
    std::vector<role> m_roles;
};

inline hello_message::hello_message()
    : m_realm()
    , m_roles()
{
}

inline hello_message::~hello_message()
{
}

inline message_type hello_message::get_type() const
{
    return message_type::Hello;
}

inline const std::string& hello_message::get_realm() const
{
    return m_realm;
}

inline const std::vector<role>& hello_message::get_roles() const
{
    return m_roles;
}

inline void hello_message::set_realm(const std::string& realm)
{
    m_realm = realm;
}

inline void hello_message::add_role(const role& r)
{
    m_roles.push_back(r);
}

} // namespace bonefish

#endif // BONEFISH_HELLO_MESSAGE_HPP
