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

#ifndef BONEFISH_NATIVE_CONNECTOR_HPP
#define BONEFISH_NATIVE_CONNECTOR_HPP

#include <bonefish/native/native_component_endpoint.hpp>
#include <bonefish/native/native_server_endpoint.hpp>

#include <functional>
#include <memory>

namespace bonefish {

/*!
 * A class that defines a connector that is shared between both the server
 * and a component to allow the component to connect/disconnect. It is the
 * server's responsibility to set the handlers in this connector and
 * make it available to components for establishing connections. The only
 * way a server can initiate a disconnect is through the component's endpoint
 * as this allows the component to define how the disconnect is handled.
 */
class native_connector
{
public:
    /*!
     * Defines a handler used by a component to create a connection with the server.
     * The component's endpoint is cached by the server so that it can communicate
     * with the component. The component endpoint must be notified when the connect
     * is complete.
     */
    using connect_handler =
            std::function<void(const std::shared_ptr<native_component_endpoint>&)>;

    /*!
     * Defines a handler used by a component to disconnect from the server. The server's
     * endpoint is passed to the handler which finds and disconnects the associated
     * connection. The component endpoint must be notified when the disconnect is complete.
     */
    using disconnect_handler =
            std::function<void(const std::shared_ptr<native_server_endpoint>&)>;

public:
    native_connector();

    connect_handler get_connect_handler() const;
    void set_connect_handler(connect_handler&& handler);

    disconnect_handler get_disconnect_handler() const;
    void set_disconnect_handler(disconnect_handler&& handler);

    /*!
     * Called by a component to create a connection with the server.
     *
     * @param endpoint The component endpoint that will be used to send
     *                 messages to the component over the connection.
     */
    void connect(const std::shared_ptr<native_component_endpoint>& endpoint);

    /*!
     * Called by a component to disconnect a from the server.
     *
     * @param endpoint The server's endpoint that the component was provided
     *                 upon a successful call to connect().
     */
    void disconnect(const std::shared_ptr<native_server_endpoint>& endpoint);

private:
    connect_handler m_connect_handler;
    disconnect_handler m_disconnect_handler;
};

inline native_connector::native_connector()
    : m_connect_handler()
    , m_disconnect_handler()
{
}

inline native_connector::connect_handler native_connector::get_connect_handler() const
{
    return m_connect_handler;
}

inline void native_connector::set_connect_handler(connect_handler&& handler)
{
    m_connect_handler = std::move(handler);
}

inline native_connector::disconnect_handler native_connector::get_disconnect_handler() const
{
    return m_disconnect_handler;
}

inline void native_connector::set_disconnect_handler(disconnect_handler&& handler)
{
    m_disconnect_handler = std::move(handler);
}

inline void native_connector::connect(const std::shared_ptr<native_component_endpoint>& endpoint)
{
    m_connect_handler(endpoint);
}

inline void native_connector::disconnect(const std::shared_ptr<native_server_endpoint>& endpoint)
{
    m_disconnect_handler(endpoint);
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_CONNECTOR_HPP
