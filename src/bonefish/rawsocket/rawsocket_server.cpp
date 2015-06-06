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

#include <bonefish/rawsocket/rawsocket_server.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/rawsocket/rawsocket_server_impl.hpp>

namespace bonefish {

rawsocket_server::rawsocket_server(
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_impl(new rawsocket_server_impl(routers, serializers))
{
}

rawsocket_server::~rawsocket_server()
{
}

void rawsocket_server::attach_listener(const std::shared_ptr<rawsocket_listener>& listener)
{
    m_impl->attach_listener(listener);
}

void rawsocket_server::start()
{
    m_impl->start();
}

void rawsocket_server::shutdown()
{
    m_impl->shutdown();
}

} // namespace bonefish
