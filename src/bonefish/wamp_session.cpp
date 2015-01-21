#include <bonefish/wamp_session.hpp>
#include <bonefish/messages/hello_message.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/welcome_message.hpp>
#include <bonefish/wamp_transport.hpp>

namespace bonefish {

wamp_session::wamp_session()
    : m_session_id()
    , m_transport()
{
}

wamp_session::wamp_session(const session_id& id,
        std::unique_ptr<wamp_transport> transport)
    : m_session_id(id)
    , m_roles()
    , m_transport(std::move(transport))
{
}

wamp_session::~wamp_session()
{
}

const session_id& wamp_session::get_session_id() const
{
    return m_session_id;
}

bool wamp_session::send_message(const wamp_message* message)
{
    return m_transport->send_message(message);
}

void wamp_session::process_hello_message(const hello_message* message)
{
    m_roles = message->get_roles();

    std::unique_ptr<welcome_message> response(new welcome_message);
    response->set_session_id(get_session_id());
    response->add_role(role(role_type::broker));
    response->add_role(role(role_type::dealer));
    send_message(response.get());
}

} // namespace bonefish
