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

#include <bonefish/native/native_endpoint.hpp>

#include <functional>
#include <memory>

namespace bonefish {

/*!
 * A class that defines a connector that is shared between both the server
 * and a component to allow the component to connect/disconnect. It is the
 * server's responsibility to set the handlers in this connector and
 * make it available to components for establishing connections.
 */
class native_connector
{
public:
    /*!
     * Defines a handler used by a component to create a connection with the server.
     * The component endpoint is passed in and cached by the server so that it can
     * communicate with the component. A future providing the server endpoint is
     * returned.
     */
    using connect_handler =
            std::function<native_endpoint_future(
                    const std::shared_ptr<native_endpoint>&)>;

    /*!
     * Defines a handler used by a component to disconnect from the server. The server
     * endpoint that was acquired when the connection was established via the connect
     * handler is passed to the disconnect handler. The server uses the server endpoint
     * to find the associated connection. A future indicating when the disconnect is
     * complete is returned.
     */
    using disconnect_handler =
            std::function<boost::future<void>(const std::shared_ptr<native_endpoint>&)>;

public:
    native_connector();

    connect_handler get_connect_handler() const;
    void set_connect_handler(connect_handler&& handler);

    disconnect_handler get_disconnect_handler() const;
    void set_disconnect_handler(disconnect_handler&& handler);

    native_endpoint_future connect(const std::shared_ptr<native_endpoint>& component_endpoint);
    boost::future<void> disconnect(const std::shared_ptr<native_endpoint>& server_endpoint);

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

inline native_endpoint_future native_connector::connect(const std::shared_ptr<native_endpoint>& component_endpoint)
{
    return m_connect_handler(component_endpoint);
}

inline boost::future<void> native_connector::disconnect(const std::shared_ptr<native_endpoint>& server_endpoint)
{
    return m_disconnect_handler(server_endpoint);
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_CONNECTOR_HPP
