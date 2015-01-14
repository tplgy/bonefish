#include <bonefish/session.hpp>

namespace bonefish {

session::session()
    : m_session_id()
    , m_handle()
    , m_server()
{
}

session::session(const session_id& id,
        const websocketpp::connection_hdl& handle,
        const std::shared_ptr<websocketpp::server<websocket_config>>& server)
    : m_session_id(id)
    , m_handle(handle)
    , m_server(server)
{
}

session::~session()
{
}

const session_id& session::get_session_id() const
{
    return m_session_id;
}

} // namespace bonefish
