#include <bonefish/wamp_session.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/wamp_transport.hpp>

namespace bonefish {

wamp_session::wamp_session()
    : m_session_id()
    , m_transport()
{
}

wamp_session::wamp_session(const wamp_session_id& id,
        std::unique_ptr<wamp_transport> transport)
    : m_session_id(id)
    , m_roles()
    , m_transport(std::move(transport))
{
}

wamp_session::~wamp_session()
{
}

const wamp_session_id& wamp_session::get_session_id() const
{
    return m_session_id;
}

bool wamp_session::send_message(const wamp_message* message)
{
    return m_transport->send_message(message);
}

void wamp_session::process_hello_message(const wamp_hello_message* hello_message)
{
    m_roles = hello_message->get_roles();

    std::unique_ptr<wamp_welcome_message> welcome_message(new wamp_welcome_message);
    welcome_message->set_session_id(get_session_id());
    welcome_message->add_role(wamp_role(wamp_role_type::BROKER));
    welcome_message->add_role(wamp_role(wamp_role_type::DEALER));
    send_message(welcome_message.get());
}

} // namespace bonefish
