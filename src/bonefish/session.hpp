#ifndef BONEFISH_SESSION_HPP
#define BONEFISH_SESSION_HPP

#include <bonefish/identifier/session_id.hpp>
#include <bonefish/websocket_config.hpp>
#include <memory>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class session
{
public:
    session();
    session(const session_id& id,
            const websocketpp::connection_hdl& handle,
            const std::shared_ptr<websocketpp::server<websocket_config>>& server);
    ~session();
    session(session const&) = delete;
    session& operator=(session const&) = delete;
    const session_id& get_session_id() const;

private:
    session_id m_session_id;
    websocketpp::connection_hdl m_handle;
    std::shared_ptr<websocketpp::server<websocket_config>> m_server;
};

} // namespace bonefish

#endif // BONEFISH_SESSION_HPP
