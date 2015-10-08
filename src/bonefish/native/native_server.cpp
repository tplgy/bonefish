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

#include <bonefish/native/native_server.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/native/native_server_impl.hpp>

namespace bonefish {

native_server::native_server(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers)
    : m_impl(new native_server_impl(io_service, routers))
{
}

native_server::~native_server()
{
}

std::shared_ptr<native_connector> native_server::get_connector() const
{
    return m_impl->get_connector();
}

void native_server::start()
{
    m_impl->start();
}

void native_server::shutdown()
{
    m_impl->shutdown();
}

} // namespace bonefish
