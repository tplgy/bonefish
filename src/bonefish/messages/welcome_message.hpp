#ifndef BONEFISH_WELCOME_MESSAGE_HPP
#define BONEFISH_WELCOME_MESSAGE_HPP

#include <bonefish/identifier/session_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <string>
#include <vector>

namespace bonefish {

//
// [WELCOME, Session|id, Details|dict]
//
class welcome_message : public wamp_message
{
public:
    welcome_message();
    ~welcome_message();

    virtual message_type get_type() const;

    const session_id& get_session_id() const;
    const std::vector<wamp_role>& get_roles() const;

    void set_session_id(const session_id& id);
    void add_role(const wamp_role& r);

private:
    session_id m_session_id;
    std::vector<wamp_role> m_roles;
};

inline welcome_message::welcome_message()
    : m_session_id()
    , m_roles()
{
}

inline welcome_message::~welcome_message()
{
}

inline message_type welcome_message::get_type() const
{
    return message_type::Welcome;
}

inline const session_id& welcome_message::get_session_id() const
{
    return m_session_id;
}

inline const std::vector<wamp_role>& welcome_message::get_roles() const
{
    return m_roles;
}

inline void welcome_message::set_session_id(const session_id& id)
{
    m_session_id = id;
}

inline void welcome_message::add_role(const wamp_role& role)
{
    m_roles.push_back(role);
}

} // namespace bonefish

#endif // BONEFISH_WELCOME_MESSAGE_HPP
