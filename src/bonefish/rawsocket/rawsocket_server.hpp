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

#ifndef BONEFISH_RAWSOCKET_SERVER_HPP
#define BONEFISH_RAWSOCKET_SERVER_HPP

#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class rawsocket_listener;
class rawsocket_server_impl;
class wamp_routers;
class wamp_serializers;

class rawsocket_server
{
public:
    rawsocket_server(
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~rawsocket_server();

    void attach_listener(const std::shared_ptr<rawsocket_listener>& listener);
    void start();
    void shutdown();

private:
    std::shared_ptr<rawsocket_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_SERVER_HPP
