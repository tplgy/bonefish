#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/wamp_routers.hpp>
#include <bonefish/websocket_server.hpp>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    std::shared_ptr<bonefish::wamp_routers> routers =
            std::make_shared<bonefish::wamp_routers>();

    std::shared_ptr<bonefish::wamp_router> realm1_router =
            std::make_shared<bonefish::wamp_router>("realm1");
    std::shared_ptr<bonefish::wamp_router> realm2_router =
            std::make_shared<bonefish::wamp_router>("realm2");

    routers->add_router(realm1_router);
    routers->add_router(realm2_router);

    std::shared_ptr<bonefish::wamp_serializers> serializers =
            std::make_shared<bonefish::wamp_serializers>();
    serializers->add_serializer(std::make_shared<bonefish::msgpack_serializer>());

    bonefish::websocket_server websocket_server(io_service, routers, serializers);
    websocket_server.start();

    io_service.run();

    return 0;
}
