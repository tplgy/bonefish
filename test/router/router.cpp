#include <bonefish/realm_routers.hpp>
#include <bonefish/websocket_server.hpp>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    std::shared_ptr<bonefish::realm_routers> routers =
            std::make_shared<bonefish::realm_routers>();

    routers->add_router(std::make_shared<bonefish::router>("realm1"));
    routers->add_router(std::make_shared<bonefish::router>("realm2"));

    bonefish::websocket_server websocket_server(io_service, routers);
    websocket_server.start();

    io_service.run();

    return 0;
}
