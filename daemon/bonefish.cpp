#include "daemon.hpp"
#include "daemon_options.hpp"

#include <boost/program_options.hpp>
#include <cstdint>
#include <iostream>

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("realm,r", po::value<std::string>(), "set the WAMP realm for this router")
        ("websocket-port,w", po::value<std::uint16_t>()->value_name("<port>"), "enable websocket transport on the given port")
        ("rawsocket-port,t", po::value<std::uint16_t>()->value_name("<port>"), "enable rawsocket transport on the given port")
        ("no-json", "disable JSON serialization")
        ("no-msgpack", "disable msgpack serialization")
    ;

    po::variables_map variables;
    bool error = false;

    try {
        po::store(po::parse_command_line(argc, argv, desc), variables);
        po::notify(variables);
    } catch (...) {
        error = true;
        std::cout << "Unrecognized options.\n\n";
    }

    if (error || variables.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    bonefish::daemon_options options;

    if (variables.count("realm")) {
        options.set_realm(variables["realm"].as<std::string>());
    }
    if (variables.count("websocket-port")) {
        options.set_websocket_enabled(true);
        options.set_websocket_port(variables["websocket-port"].as<std::uint16_t>());
    }
    if (variables.count("rawsocket-port")) {
        options.set_rawsocket_enabled(true);
        options.set_rawsocket_port(variables["rawsocket-port"].as<std::uint16_t>());
    }
    if (variables.count("no-json")) {
        options.set_json_serialization_enabled(false);
    }
    if (variables.count("no-msgpack")) {
        options.set_msgpack_serialization_enabled(false);
    }

    bonefish::daemon daemon(options);
    daemon.run();
    return 0;
}
