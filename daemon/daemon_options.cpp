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

#include "daemon_options.hpp"

namespace bonefish {

daemon_options::daemon_options()
    : m_realm()
    , m_debug_enabled(false)
    , m_websocket_port(0)
    , m_rawsocket_port(0)
    , m_rawsocket_path()
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
    if ((m_rawsocket_enabled && m_rawsocket_port == 0) &&
            (m_rawsocket_enabled && m_rawsocket_path.empty())) {
        list.push_back("Rawsocket support is enabled but no tcp port or uds path is set.");
    }
    return list;
}

} // namespace bonefish

