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

#ifndef BONEFISH_DAEMON_HPP
#define BONEFISH_DAEMON_HPP

#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/websocket/websocket_server.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <cstdint>
#include <memory>

namespace bonefish {

class daemon_options;
class rawsocket_server;
class wamp_routers;
class wamp_serializers;
class websocket_server;

class daemon
{
public:
    daemon(const daemon_options& options);
    ~daemon();

    void run();
    void shutdown();
    boost::asio::io_service& io_service();

private:
    void shutdown_handler();
    void termination_signal_handler(
            const boost::system::error_code& error_code, int signal_number);

private:
    boost::asio::io_service m_io_service;
    std::shared_ptr<boost::asio::io_service::work> m_work;
    boost::asio::signal_set m_termination_signals;

    std::shared_ptr<bonefish::wamp_routers> m_routers;
    std::shared_ptr<bonefish::wamp_serializers> m_serializers;
    std::shared_ptr<bonefish::rawsocket_server> m_rawsocket_server;
    std::shared_ptr<bonefish::websocket_server> m_websocket_server;

    std::uint16_t m_websocket_port;
};

} // namespace bonefish

#endif // BONEFISH_DAEMON_H
