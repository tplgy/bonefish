#ifndef BONEFISH_SESSION_WAMP_SESSION_HPP
#define BONEFISH_SESSION_WAMP_SESSION_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/session/wamp_session_state.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/websocket/websocket_config.hpp>
#include <memory>
#include <string>
#include <unordered_set>
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

    wamp_session_state get_state() const;
    const wamp_session_id& get_session_id() const;
    const std::unique_ptr<wamp_transport>& get_transport() const;

    void set_state(wamp_session_state session_state);
    void set_roles(const std::unordered_set<wamp_role>& roles);

private:
    wamp_session_id m_session_id;
    wamp_session_state m_session_state;
    std::unordered_set<wamp_role> m_roles;
    std::unique_ptr<wamp_transport> m_transport;
};

inline wamp_session::wamp_session()
    : m_session_id()
    , m_session_state(wamp_session_state::NONE)
    , m_roles()
    , m_transport()
{
}

inline wamp_session::wamp_session(const wamp_session_id& id,
        std::unique_ptr<wamp_transport> transport)
    : m_session_id(id)
    , m_session_state(wamp_session_state::NONE)
    , m_roles()
    , m_transport(std::move(transport))
{
}

inline wamp_session::~wamp_session()
{
}

inline const wamp_session_id& wamp_session::get_session_id() const
{
    return m_session_id;
}

inline const std::unique_ptr<wamp_transport>& wamp_session::get_transport() const
{
    return m_transport;
}

inline void wamp_session::set_roles(const std::unordered_set<wamp_role>& roles)
{
    m_roles.clear();
    m_roles.insert(roles.begin(), roles.end());
}

inline void wamp_session::set_state(wamp_session_state state)
{
    m_session_state = state;
}

inline wamp_session_state wamp_session::get_state() const
{
    return m_session_state;
}

} // namespace bonefish

#endif // BONEFISH_SESSION_WAMP_SESSION_HPP
