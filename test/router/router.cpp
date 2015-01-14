#include <bonefish/router.hpp>
#include <bonefish/websocket_server.hpp>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    std::shared_ptr<bonefish::router> router =
            std::make_shared<bonefish::router>();

    std::shared_ptr<bonefish::websocket_server> websocket_server =
            std::make_shared<bonefish::websocket_server>(router);

    std::thread websocket_server_thread(
            std::bind(&bonefish::websocket_server::run, websocket_server));

    router->run();

    websocket_server_thread.join();

    return 0;
}
