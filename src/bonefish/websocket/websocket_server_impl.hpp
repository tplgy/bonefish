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

#ifndef BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_IMPL_HPP
#define BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_IMPL_HPP

#include <bonefish/common/wamp_message_processor.hpp>
#include <bonefish/websocket/websocket_config.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <websocketpp/server.hpp>

namespace bonefish {

class wamp_routers;
class wamp_serializers;

class websocket_server_impl : public std::enable_shared_from_this<websocket_server_impl>
{
public:
    websocket_server_impl(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~websocket_server_impl();

    void start(const boost::asio::ip::address& ip_address, uint16_t port);
    void shutdown();

private:
    void on_socket_init(websocketpp::connection_hdl handle,
            boost::asio::ip::tcp::socket& socket);
    void on_open(websocketpp::connection_hdl handle);
    void on_close(websocketpp::connection_hdl handle);
    void on_fail(websocketpp::connection_hdl handle);
    bool on_validate(websocketpp::connection_hdl handle);
    void on_message(websocketpp::connection_hdl handle,
            websocketpp::server<websocket_config>::message_ptr message);

private:
    boost::asio::io_service& m_io_service;
    std::shared_ptr<websocketpp::server<websocket_config>> m_server;
    std::shared_ptr<wamp_routers> m_routers;
    std::shared_ptr<wamp_serializers> m_serializers;
    wamp_message_processor m_message_processor;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_IMPL_HPP
