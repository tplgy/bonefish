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

#ifndef BONEFISH_NATIVE_SERVER_HPP
#define BONEFISH_NATIVE_SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <memory>

namespace bonefish {

class native_connector;
class native_server_impl;
class wamp_routers;

/*!
 * A class that provides a synthesized server designed to allow for native
 * transports to be utilized. Native transports allow for an in process
 * mechanism for components to connect and perform all manner of WAMP
 * operations without the need for sockets or any other type of IPC
 * mechanism.
 */
class native_server
{
public:
    native_server(
            boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers);

    ~native_server();

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
     * The underlying server implementation.
     */
    std::shared_ptr<native_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_NATIVE_SERVER_HPP
