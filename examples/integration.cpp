#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/tcp/tcp_server.hpp>
#include <bonefish/websocket/websocket_server.hpp>

// In some cases you may need to integrate bonefish with your application
// such that it is running in the same process space. This serves as a simple
// example of how to use the bonefish library to startup your own instance
// natively. As the library evolves, this example will be updated.

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    std::shared_ptr<bonefish::wamp_routers> routers =
            std::make_shared<bonefish::wamp_routers>();

    std::shared_ptr<bonefish::wamp_router> router =
            std::make_shared<bonefish::wamp_router>(io_service, "default");

    routers->add_router(router);

    std::shared_ptr<bonefish::wamp_serializers> serializers =
            std::make_shared<bonefish::wamp_serializers>();
    serializers->add_serializer(std::make_shared<bonefish::msgpack_serializer>());

    std::shared_ptr<bonefish::wamp_session_id_generator> generator =
            std::make_shared<bonefish::wamp_session_id_generator>();

    std::shared_ptr<bonefish::tcp_server> tcp_server =
            std::make_shared<bonefish::tcp_server>(io_service, routers, serializers, generator);
    tcp_server->start();

    std::shared_ptr<bonefish::websocket_server> websocket_server =
            std::make_shared<bonefish::websocket_server>(io_service, routers, serializers, generator);
    websocket_server->start();

    io_service.run();

    return 0;
}
