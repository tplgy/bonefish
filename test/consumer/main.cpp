// Smoke test for consuming bonefish from an independent CMake project:
// every public header must compile against the installed, self-contained
// header set, and the exported bonefish::bonefish target must link. The
// runtime half exercises a native connect/disconnect round-trip.

#include <bonefish/native/native_connector.hpp>
#include <bonefish/native/native_endpoint.hpp>
#include <bonefish/native/native_server.hpp>
#include <bonefish/rawsocket/rawsocket_server.hpp>
#include <bonefish/rawsocket/tcp_listener.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/json_serializer.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/trace/trace.hpp>
#include <bonefish/websocket/websocket_server.hpp>

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>

#include <cstdio>
#include <future>
#include <memory>
#include <thread>

using namespace bonefish;

int main()
{
    int failures = 0;

    asio::io_context io_service;
    auto work = asio::make_work_guard(io_service);
    std::thread io_thread([&io_service]() { io_service.run(); });

    {
        auto routers = std::make_shared<wamp_routers>();
        routers->add_router(std::make_shared<wamp_router>(io_service, "default"));

        auto serializers = std::make_shared<wamp_serializers>();
        serializers->add_serializer(std::make_shared<msgpack_serializer>());
        serializers->add_serializer(std::make_shared<json_serializer>());

        // Servers must at least construct against the installed headers/lib.
        rawsocket_server rawsocket(routers, serializers);
        websocket_server websocket(io_service, routers, serializers);

        native_server server(io_service, routers);
        server.start();

        auto connector = server.get_connector();
        auto connect_future = connector->connect(std::make_shared<native_endpoint>());
        if (!connect_future.get()) {
            std::fprintf(stderr, "native connect returned null endpoint\n");
            ++failures;
        }

        server.shutdown();
    }

    work.reset();
    io_thread.join();

    if (failures) {
        std::fprintf(stderr, "consumer smoke test FAILED (%d)\n", failures);
        return 1;
    }
    std::printf("consumer smoke test OK\n");
    return 0;
}
