/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef BONEFISH_SESSION_WAMP_SESSION_HPP
#define BONEFISH_SESSION_WAMP_SESSION_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/session/wamp_session_state.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/websocket/websocket_config.hpp>

#include <memory>
#include <ostream>
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
    wamp_session(
            const wamp_session_id& id,
            const std::string& realm,
            std::unique_ptr<wamp_transport> transport);
    ~wamp_session();
    wamp_session(wamp_session const&) = delete;
    wamp_session& operator=(wamp_session const&) = delete;

    const std::string& get_realm() const;
    wamp_session_state get_state() const;
    const wamp_role* get_role(wamp_role_type type) const;
    const std::unordered_set<wamp_role>& get_roles() const;
    const wamp_session_id& get_session_id() const;
    const std::unique_ptr<wamp_transport>& get_transport() const;

    void set_state(wamp_session_state session_state);
    void set_roles(const std::unordered_set<wamp_role>& roles);
    void set_roles(std::unordered_set<wamp_role>&& roles);
    void add_role(const wamp_role& role);
    void add_role(wamp_role&& role);

private:
    const std::string m_realm;
    wamp_session_id m_session_id;
    wamp_session_state m_session_state;
    std::unordered_set<wamp_role> m_roles;
    std::unique_ptr<wamp_transport> m_transport;
};

inline wamp_session::wamp_session()
    : m_realm()
    , m_session_id()
    , m_session_state(wamp_session_state::NONE)
    , m_roles()
    , m_transport()
{
}

inline wamp_session::wamp_session(
        const wamp_session_id& id,
        const std::string& realm,
        std::unique_ptr<wamp_transport> transport)
    : m_realm(realm)
    , m_session_id(id)
    , m_session_state(wamp_session_state::NONE)
    , m_roles()
    , m_transport(std::move(transport))
{
}

inline wamp_session::~wamp_session()
{
}

inline const std::string& wamp_session::get_realm() const
{
    return m_realm;
}

inline wamp_session_state wamp_session::get_state() const
{
    return m_session_state;
}

inline const wamp_role* wamp_session::get_role(wamp_role_type type) const
{
    for (const auto& role : m_roles) {
        if (role.get_type() == type) {
            return &role;
        }
    }

    return nullptr;
}

inline const std::unordered_set<wamp_role>& wamp_session::get_roles() const
{
    return m_roles;
}

inline const wamp_session_id& wamp_session::get_session_id() const
{
    return m_session_id;
}

inline const std::unique_ptr<wamp_transport>& wamp_session::get_transport() const
{
    return m_transport;
}

inline void wamp_session::set_state(wamp_session_state state)
{
    m_session_state = state;
}

inline void wamp_session::set_roles(const std::unordered_set<wamp_role>& roles)
{
    m_roles = roles;
}

inline void wamp_session::set_roles(std::unordered_set<wamp_role>&& roles)
{
    m_roles = std::move(roles);
}

inline void wamp_session::add_role(const wamp_role& role)
{
    m_roles.insert(role);
}

inline void wamp_session::add_role(wamp_role&& role)
{
    m_roles.insert(std::move(role));
}

inline std::ostream& operator<<(std::ostream& os, const wamp_session& session)
{
    os << "session [" << session.get_session_id() << ","
            << session.get_realm() << "," << session.get_state() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_SESSION_WAMP_SESSION_HPP
