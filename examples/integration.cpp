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
#include <bonefish/rawsocket/tcp_listener.hpp>
#include <bonefish/rawsocket/uds_listener.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/trace/trace.hpp>
#include <bonefish/websocket/websocket_server.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <memory>
#include <stdlib.h>

// In some cases you may need to integrate bonefish with your application
// such that it is running in the same process space. This serves as a simple
// example of how to use the bonefish library to startup your own instance
// natively. As the library evolves, this example will be updated.

int main(int argc, char** argv)
{
    if (getenv("BONEFISH_TRACE")) {
        bonefish::trace::set_enabled(true);
    }

    boost::asio::io_service io_service;

    std::shared_ptr<bonefish::wamp_routers> routers =
            std::make_shared<bonefish::wamp_routers>();

    std::shared_ptr<bonefish::wamp_router> router =
            std::make_shared<bonefish::wamp_router>(io_service, "default");

    routers->add_router(router);

    std::shared_ptr<bonefish::wamp_serializers> serializers =
            std::make_shared<bonefish::wamp_serializers>();
    serializers->add_serializer(std::make_shared<bonefish::msgpack_serializer>());

    std::shared_ptr<bonefish::rawsocket_server> rawsocket_server =
            std::make_shared<bonefish::rawsocket_server>(routers, serializers);
    std::shared_ptr<bonefish::uds_listener> uds_listener =
            std::make_shared<bonefish::uds_listener>(io_service, "/tmp/bonefish.sock");
    std::shared_ptr<bonefish::tcp_listener> tcp_listener =
            std::make_shared<bonefish::tcp_listener>(io_service, boost::asio::ip::address(), 8888);

    rawsocket_server->attach_listener(
            std::static_pointer_cast<bonefish::rawsocket_listener>(uds_listener));
    rawsocket_server->attach_listener(
            std::static_pointer_cast<bonefish::rawsocket_listener>(tcp_listener));
    rawsocket_server->start();

    std::shared_ptr<bonefish::websocket_server> websocket_server =
            std::make_shared<bonefish::websocket_server>(io_service, routers, serializers);
    websocket_server->start(boost::asio::ip::address(), 9999);

    io_service.run();

    return 0;
}
