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

#include <bonefish/websocket/websocket_server.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/websocket/websocket_server_impl.hpp>

namespace bonefish {

websocket_server::websocket_server(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_impl(std::make_shared<websocket_server_impl>(io_service, routers, serializers))
{
}

websocket_server::~websocket_server()
{
}

void websocket_server::start(const boost::asio::ip::address& ip_address, uint16_t port)
{
    m_impl->start(ip_address, port);
}

void websocket_server::shutdown()
{
    m_impl->shutdown();
}

} // namespace bonefish
