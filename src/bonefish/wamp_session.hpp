#ifndef BONEFISH_WAMP_SESSION_HPP
#define BONEFISH_WAMP_SESSION_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/websocket_config.hpp>
#include <memory>
#include <string>
#include <vector>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class wamp_hello_message;
class wamp_message;
class wamp_transport;

class wamp_session
{
public:
    wamp_session();
    wamp_session(const wamp_session_id& id,
            std::unique_ptr<wamp_transport> transport);
    ~wamp_session();
    wamp_session(wamp_session const&) = delete;
    wamp_session& operator=(wamp_session const&) = delete;
    const wamp_session_id& get_session_id() const;
    bool send_message(const wamp_message* message);

    void process_hello_message(const wamp_hello_message* hello_message);

private:
    wamp_session_id m_session_id;
    std::vector<wamp_role> m_roles;
    std::unique_ptr<wamp_transport> m_transport;
};

} // namespace bonefish

#endif // BONEFISH_WAMP_SESSION_HPP
