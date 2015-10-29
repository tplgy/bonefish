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

#ifndef BONEFISH_NATIVE_SERVER_IMPL_HPP
#define BONEFISH_NATIVE_SERVER_IMPL_HPP

#include <bonefish/common/wamp_message_processor.hpp>
#include <bonefish/native/native_connector.hpp>

#include <boost/asio/io_service.hpp>
#include <map>
#include <memory>
#include <msgpack.hpp>
#include <set>
#include <vector>

namespace bonefish {

class native_connection;
class native_endpoint;
class wamp_routers;

/*!
 * A class representing the implementation of a native server.
 */
class native_server_impl :
        public std::enable_shared_from_this<native_server_impl>
{
public:
    native_server_impl(
            boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers);

    ~native_server_impl();

    /*!
     * Retrieves the connector that components can use to connect to the server.
     *
     * @return The connector that components can use to connect to the server.
     */
    std::shared_ptr<native_connector> get_connector() const;

    /*!
     * Starts the server.
     */
    void start();

    /*!
     * Shuts down the server.
     */
    void shutdown();

private:
    /*!
     * Callback handler that is invoked when a component is establishing a
     * connection with the server.
     *
     * @param component_endpoint The endpoint of the component that is establishing
     *                           the connection.
     *
     * @return The server endpoint associated with the connection
     */
    std::shared_ptr<native_endpoint> on_connect(
            const std::shared_ptr<native_endpoint>& component_endpoint);

    /*!
     * Callback handler that is invoked when a component is establishing a
     * connection with the server.
     *
     * @param server_endpoint The endpoint of the component that is establishing
     *                        the connection.
     */
    void on_disconnect(const std::shared_ptr<native_endpoint>& server_endpoint);

    /*!
     * Callback handler that is invoked when a message has been received.
     *
     * @param connection The connection that the message arrived on.
     * @param fields The fields representing the message.
     * @param zone The zone the message fields were allocated from.
     */
    void on_message(
            const std::shared_ptr<native_connection>& connection,
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone);

private:
    /*!
     * The io service to drive all events.
     */
    boost::asio::io_service& m_io_service;

    /*!
     * The routers serviced by this server.
     */
    std::shared_ptr<wamp_routers> m_routers;

    /*!
     * The connector that components can use to connect to this server.
     */
    std::shared_ptr<native_connector> m_connector;

    /*!
     * The current set of active connections.
     */
    std::set<std::shared_ptr<native_connection>,
            std::owner_less<std::shared_ptr<native_connection>>> m_connections;

    /*!
     * A mapping of server endpoints to connections. Components only know
     * about server endpoints so this allows us to perform the reverse mapping
     * when required.
     */
    std::map<std::shared_ptr<native_endpoint>,
            std::shared_ptr<native_connection>,
            std::owner_less<std::shared_ptr<native_endpoint>>> m_endpoints_connected;

    /*!
     * The message processor to use for feeding messages into the routers.
     */
    wamp_message_processor m_message_processor;
};

inline std::shared_ptr<native_connector> native_server_impl::get_connector() const
{
    return m_connector;
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_SERVER_IMPL_HPP
