// Regression tests for the native server connect/disconnect handling:
//  - disconnect of a live endpoint must complete without exception
//    (previously the inverted weak_ptr check made every disconnect fail)
//  - disconnect of an unknown endpoint must surface std::invalid_argument
//    through the returned future (previously the missing throw caused
//    dereferencing of an end() iterator).

#include "bonefish_test.hpp"

#include <bonefish/native/native_connector.hpp>
#include <bonefish/native/native_endpoint.hpp>
#include <bonefish/native/native_server.hpp>
#include <bonefish/router/wamp_routers.hpp>

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>

#include <chrono>
#include <cstdio>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>

using namespace bonefish;

int main()
{
    asio::io_context io_service;
    auto work = asio::make_work_guard(io_service);
    std::thread io_thread([&io_service]() { io_service.run(); });

    int failures = 0;

    {
        auto routers = std::make_shared<wamp_routers>();
        native_server server(io_service, routers);
        server.start();

        auto connector = server.get_connector();
        CHECK(connector != nullptr);

        auto component_endpoint = std::make_shared<native_endpoint>();
        auto connect_future = connector->connect(component_endpoint);

        std::shared_ptr<native_endpoint> server_endpoint;
        try {
            server_endpoint = connect_future.get();
        } catch (const std::exception& e) {
            CHECK(false); // connect failed: unexpected
        }
        CHECK(server_endpoint != nullptr);

        if (server_endpoint) {
            // Regression for the inverted shared_this check: a normal
            // disconnect of a live endpoint must fulfill the future without
            // an exception.
            auto disconnect_future = connector->disconnect(server_endpoint);
            bool threw = false;
            std::string what;
            try {
                disconnect_future.get();
            } catch (const std::exception& e) {
                threw = true;
                what = e.what();
            }
            CHECK(!threw);
            if (threw) {
                std::fprintf(stderr, "unexpected disconnect exception: %s\n", what.c_str());
            }
        }

        // Regression for the missing throw: disconnecting an endpoint the
        // server does not know must report std::invalid_argument via the
        // future rather than dereferencing the end() iterator.
        {
            auto disconnect_future =
                    connector->disconnect(std::make_shared<native_endpoint>());
            bool threw_invalid = false;
            try {
                disconnect_future.get();
            } catch (const std::invalid_argument&) {
                threw_invalid = true;
            } catch (const std::exception&) {
                CHECK(false); // wrong exception type
            }
            CHECK(threw_invalid);
        }

        server.shutdown();
    }

    work.reset();
    io_thread.join();

    return test_summary("native_disconnect");
}
