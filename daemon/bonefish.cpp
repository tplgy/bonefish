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

#include "daemon.hpp"
#include "daemon_options.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace {

void print_usage()
{
    std::cout << "Allowed options:\n"
              << "  -h [ --help ]            produce help message\n"
              << "  -r [ --realm ] arg       set the WAMP realm for this router\n"
              << "  -w [ --websocket-port ] arg\n"
              << "                           enable websocket transport on the "
                 "given port\n"
              << "  -t [ --rawsocket-port ] arg\n"
              << "                           enable rawsocket transport on the "
                 "given port\n"
              << "  -u [ --rawsocket-path ] arg\n"
              << "                           enable rawsocket transport on the "
                 "given path\n"
              << "  --no-json                disable JSON serialization\n"
              << "  --no-msgpack             disable msgpack serialization\n"
              << "  -d [ --debug ]           enable debugging\n";
}

bool parse_port(const std::string& value, std::uint16_t& port)
{
    if (value.empty()) {
        return false;
    }
    char* end = nullptr;
    const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
    if (end == value.c_str() || *end != '\0' || parsed > 0xFFFF) {
        return false;
    }
    port = static_cast<std::uint16_t>(parsed);
    return true;
}

struct option_specs {
    const char* long_name;
    char short_name;
    bool has_value;
};

const option_specs OPTION_TABLE[] = {
    { "help", 'h', false },
    { "realm", 'r', true },
    { "websocket-port", 'w', true },
    { "rawsocket-port", 't', true },
    { "rawsocket-path", 'u', true },
    { "no-json", '\0', false },
    { "no-msgpack", '\0', false },
    { "debug", 'd', false }
};

} // namespace

int main(int argc, char** argv)
{
    bonefish::daemon_options options;

    bool help_requested = false;
    bool error = false;
    bool debug_enabled = false;

    for (int i = 1; i < argc && !error; ++i) {
        std::string arg(argv[i]);

        std::string name;
        std::string value;
        bool has_value_argument = false;

        if (arg.rfind("--", 0) == 0) {
            name = arg.substr(2);
            const auto equals = name.find('=');
            if (equals != std::string::npos) {
                value = name.substr(equals + 1);
                name = name.substr(0, equals);
                has_value_argument = true;
            }
        } else if (arg.size() > 1 && arg[0] == '-') {
            name = std::string(1, arg[1]);
            if (arg.size() > 2) {
                value = arg.substr(2);
                has_value_argument = true;
            }
        } else {
            error = true;
            break;
        }

        const option_specs* spec = nullptr;
        for (const auto& candidate : OPTION_TABLE) {
            if (name == candidate.long_name ||
                (candidate.short_name != '\0' && name == std::string(1, candidate.short_name))) {
                spec = &candidate;
                break;
            }
        }

        if (spec == nullptr) {
            error = true;
            break;
        }

        if (std::strcmp(spec->long_name, "help") == 0) {
            help_requested = true;
            continue;
        }

        if (spec->has_value && !has_value_argument) {
            if (i + 1 >= argc) {
                error = true;
                break;
            }
            value = argv[++i];
        }

        if (std::strcmp(spec->long_name, "realm") == 0) {
            options.set_realm(value);
        } else if (std::strcmp(spec->long_name, "websocket-port") == 0) {
            std::uint16_t port;
            if (!parse_port(value, port)) {
                error = true;
                break;
            }
            options.set_websocket_enabled(true);
            options.set_websocket_port(port);
        } else if (std::strcmp(spec->long_name, "rawsocket-port") == 0) {
            std::uint16_t port;
            if (!parse_port(value, port)) {
                error = true;
                break;
            }
            options.set_rawsocket_enabled(true);
            options.set_rawsocket_port(port);
        } else if (std::strcmp(spec->long_name, "rawsocket-path") == 0) {
            options.set_rawsocket_enabled(true);
            options.set_rawsocket_path(value);
        } else if (std::strcmp(spec->long_name, "no-json") == 0) {
            options.set_json_serialization_enabled(false);
        } else if (std::strcmp(spec->long_name, "no-msgpack") == 0) {
            options.set_msgpack_serialization_enabled(false);
        } else if (std::strcmp(spec->long_name, "debug") == 0) {
            debug_enabled = true;
        }
    }

    if (error) {
        std::cout << "Unrecognized options.\n\n";
    }

    if (error || help_requested) {
        print_usage();
        return 1;
    }

    options.set_debug_enabled(debug_enabled);

    bonefish::daemon daemon(options);
    daemon.run();
    return 0;
}
