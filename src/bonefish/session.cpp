#include <bonefish/session.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/session_transport.hpp>

namespace bonefish {

session::session()
    : m_session_id()
    , m_transport()
{
}

session::session(const session_id& id,
        std::unique_ptr<session_transport> transport)
    : m_session_id(id)
    , m_transport(std::move(transport))
{
}

session::~session()
{
}

const session_id& session::get_session_id() const
{
    return m_session_id;
}

bool session::send_message(const wamp_message* message)
{
    return m_transport->send_message(message);
}

} // namespace bonefish
