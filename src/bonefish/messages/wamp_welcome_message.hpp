#ifndef BONEFISH_WAMP_WELCOME_MESSAGE_HPP
#define BONEFISH_WAMP_WELCOME_MESSAGE_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <string>
#include <vector>

namespace bonefish {

//
// [WELCOME, Session|id, Details|dict]
//
class wamp_welcome_message : public wamp_message
{
public:
    wamp_welcome_message();
    virtual ~wamp_welcome_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_session_id& get_session_id() const;
    const std::vector<wamp_role>& get_roles() const;

    void set_session_id(const wamp_session_id& id);
    void add_role(const wamp_role& r);

private:
    wamp_session_id m_session_id;
    std::vector<wamp_role> m_roles;
};

inline wamp_welcome_message::wamp_welcome_message()
    : m_session_id()
    , m_roles()
{
}

inline wamp_welcome_message::~wamp_welcome_message()
{
}

inline wamp_message_type wamp_welcome_message::get_type() const
{
    return wamp_message_type::WELCOME;
}

inline const wamp_session_id& wamp_welcome_message::get_session_id() const
{
    return m_session_id;
}

inline const std::vector<wamp_role>& wamp_welcome_message::get_roles() const
{
    return m_roles;
}

inline void wamp_welcome_message::set_session_id(const wamp_session_id& id)
{
    m_session_id = id;
}

inline void wamp_welcome_message::add_role(const wamp_role& role)
{
    m_roles.push_back(role);
}

} // namespace bonefish

#endif // BONEFISH_WAMP_WELCOME_MESSAGE_HPP
