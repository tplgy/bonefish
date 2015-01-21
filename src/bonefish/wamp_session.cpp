#include <bonefish/wamp_session.hpp>
#include <bonefish/messages/wamp_message.hpp>
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

} // namespace bonefish
