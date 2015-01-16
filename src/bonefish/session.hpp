#ifndef BONEFISH_SESSION_HPP
#define BONEFISH_SESSION_HPP

#include <bonefish/identifier/session_id.hpp>
#include <bonefish/websocket_config.hpp>
#include <memory>
#include <string>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class session_transport;

class session
{
public:
    session();
    session(const session_id& id,
            std::unique_ptr<session_transport> transport);
    ~session();
    session(session const&) = delete;
    session& operator=(session const&) = delete;
    const session_id& get_session_id() const;

private:
    session_id m_session_id;
    std::unique_ptr<session_transport> m_transport;
};

} // namespace bonefish

#endif // BONEFISH_SESSION_HPP
