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

#ifndef BONEFISH_RAWSOCKET_SERVER_IMPL_HPP
#define BONEFISH_RAWSOCKET_SERVER_IMPL_HPP

#include <bonefish/common/wamp_message_processor.hpp>

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class rawsocket_connection;
class rawsocket_listener;
class wamp_routers;
class wamp_serializers;

class rawsocket_server_impl : public std::enable_shared_from_this<rawsocket_server_impl>
{
public:
    rawsocket_server_impl(
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~rawsocket_server_impl();

    void attach_listener(const std::shared_ptr<rawsocket_listener>& listener);
    void start();
    void shutdown();

private:
    void on_connect(const std::shared_ptr<rawsocket_connection>& connection);
    void on_handshake(const std::shared_ptr<rawsocket_connection>& connection,
            uint32_t capabilities);
    void on_message(const std::shared_ptr<rawsocket_connection>& connection,
            const char* buffer, size_t length);
    void on_close(const std::shared_ptr<rawsocket_connection>& connection);
    void on_fail(const std::shared_ptr<rawsocket_connection>& connection, const char* reason);

    void teardown_connection(const std::shared_ptr<rawsocket_connection>& connection);

private:
    std::shared_ptr<wamp_routers> m_routers;
    std::shared_ptr<wamp_serializers> m_serializers;

    std::set<std::shared_ptr<rawsocket_listener>,
            std::owner_less<std::shared_ptr<rawsocket_listener>>> m_listeners;

    std::set<std::shared_ptr<rawsocket_connection>,
            std::owner_less<std::shared_ptr<rawsocket_connection>>> m_connections;

    wamp_message_processor m_message_processor;
};

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_SERVER_IMPL_HPP
