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

#ifndef BONEFISH_NATIVE_COMPONENT_ENDPOINT_HPP
#define BONEFISH_NATIVE_COMPONENT_ENDPOINT_HPP

#include <bonefish/native/native_endpoint.hpp>
#include <bonefish/native/native_server_endpoint.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

/*!
 * A class that provides the concept of an endpoint for communicating natively
 * between a component and a server. Communication is message based and conducted
 * in a zero copy manner, as ownership of the message is passed via the endpoint.
 */
class native_component_endpoint :
        public native_endpoint,
        public std::enable_shared_from_this<native_component_endpoint>
{
public:
    /*!
     * Defines a handler to be invoked by a server to indicate that a component
     * initiated connection with the server has been created. This allows a
     * component to synchronize with the server when creating the initial connection.
     * The server's endpoint is passed to this handler which then allows the component
     * to start sending messsages to the server.
     */
    using connected_handler =
            std::function<void(const std::shared_ptr<native_server_endpoint>&)>;

    /*!
     * Defines a handler to be invoked by the server to indicate that a component
     * initiated disconnect with the server has been processed. This allows a
     * component to synchonize with the server when disconnecting.
     */
    using disconnected_handler = std::function<void()>;

    /*!
     * Defines a handler to be invoked by the server to indicate that it wants
     * to disconnect from the component. This allows a server to synchonize with
     * the component when disconnecting. The component is required to notify the
     * server endpoint via its `disconnected_handler` when it is complete.
     */
    using disconnect_handler = std::function<void()>;

public:
    native_component_endpoint();

    const connected_handler& get_connected_handler() const;
    void set_connected_handler(connected_handler&& handler);

    const disconnected_handler& get_disconnected_handler() const;
    void set_disconnected_handler(disconnected_handler&& handler);

    const disconnect_handler& get_disconnect_handler() const;
    void set_disconnect_handler(disconnected_handler&& handler);

private:
    connected_handler m_connected_handler;
    disconnected_handler m_disconnected_handler;
    disconnect_handler m_disconnect_handler;
};

inline native_component_endpoint::native_component_endpoint()
    : m_connected_handler()
    , m_disconnected_handler()
    , m_disconnect_handler()
{
}

inline const native_component_endpoint::connected_handler&
native_component_endpoint::get_connected_handler() const
{
    return m_connected_handler;
}

inline void native_component_endpoint::set_connected_handler(
        connected_handler&& handler)
{
    m_connected_handler = std::move(handler);
}

inline const native_component_endpoint::disconnected_handler&
native_component_endpoint::get_disconnected_handler() const
{
    return m_disconnected_handler;
}

inline void native_component_endpoint::set_disconnected_handler(
        disconnected_handler&& handler)
{
    m_disconnected_handler = std::move(handler);
}

inline const native_component_endpoint::disconnect_handler&
native_component_endpoint::get_disconnect_handler() const
{
    return m_disconnect_handler;
}

inline void native_component_endpoint::set_disconnect_handler(
        disconnect_handler&& handler)
{
    m_disconnect_handler = std::move(handler);
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_COMPONENT_ENDPOINT_HPP
