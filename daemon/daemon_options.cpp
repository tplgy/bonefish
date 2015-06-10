#include "daemon_options.hpp"

namespace bonefish {

daemon_options::daemon_options()
    : m_realm()
    , m_debug_enabled(false)
    , m_websocket_port(0)
    , m_rawsocket_port(0)
    , m_websocket_enabled(false)
    , m_rawsocket_enabled(false)
    , m_json_serialization_enabled(true)
    , m_msgpack_serialization_enabled(true)
{
}

std::vector<std::string> daemon_options::problems() const
{
    std::vector<std::string> list;

    if (m_realm.empty()) {
        list.push_back("No realm was given.");
    }
    if (!m_websocket_enabled && !m_rawsocket_enabled) {
        list.push_back("No transports are enabled.");
    }
    if (!m_json_serialization_enabled && !m_msgpack_serialization_enabled) {
        list.push_back("No serialization methods are enabled.");
    }
    if (m_websocket_enabled && m_websocket_port == 0) {
        list.push_back("Websocket support is enabled but no port is set.");
    }
    if (m_rawsocket_enabled && m_rawsocket_port == 0) {
        list.push_back("Rawsocket support is enabled but no port is set.");
    }
    return list;
}

} // namespace bonefish

