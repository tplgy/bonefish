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

#ifndef BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP
#define BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstdint>
#include <memory>

namespace bonefish {

class wamp_routers;
class wamp_serializers;
class websocket_server_impl;

class websocket_server
{
public:
    websocket_server(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~websocket_server();

    void start(const boost::asio::ip::address& ip_address, uint16_t port);
    void shutdown();

private:
    std::shared_ptr<websocket_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP
