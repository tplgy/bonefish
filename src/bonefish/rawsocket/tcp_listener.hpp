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

#ifndef BONEFISH_TCP_LISTENER_HPP
#define BONEFISH_TCP_LISTENER_HPP

#include <bonefish/rawsocket/rawsocket_listener.hpp>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdint>
#include <memory>

namespace bonefish {

class tcp_listener :
        public rawsocket_listener,
        public std::enable_shared_from_this<tcp_listener>
{
public:
    tcp_listener(
            asio::io_context& io_service,
            const asio::ip::address& ip_address,
            uint16_t port);
    virtual ~tcp_listener() override;

    virtual void start_listening() override;
    virtual void stop_listening() override;
    virtual std::shared_ptr<rawsocket_connection> create_connection() override;

protected:
    virtual void async_accept() override;

private:
    asio::ip::tcp::socket m_socket;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::endpoint m_endpoint;
};

} // namespace bonefish

#endif // BONEFISH_TCP_LISTENER_HPP
