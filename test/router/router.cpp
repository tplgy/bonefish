#include <bonefish/realm_routers.hpp>
#include <bonefish/websocket_server.hpp>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    std::shared_ptr<bonefish::realm_routers> routers =
            std::make_shared<bonefish::realm_routers>();

    routers->add_router(std::make_shared<bonefish::router>("realm1"));
    routers->add_router(std::make_shared<bonefish::router>("realm2"));

    std::shared_ptr<bonefish::websocket_server> websocket_server =
            std::make_shared<bonefish::websocket_server>(routers);

    std::thread websocket_server_thread(
            std::bind(&bonefish::websocket_server::run, websocket_server));

    websocket_server_thread.join();

    return 0;
}
