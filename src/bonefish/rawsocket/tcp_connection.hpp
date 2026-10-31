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

#ifndef BONEFISH_TCP_CONNECTION_HPP
#define BONEFISH_TCP_CONNECTION_HPP

#include <bonefish/rawsocket/rawsocket_connection.hpp>

#include <asio/buffer.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

namespace bonefish {

class tcp_connection :
        public rawsocket_connection
{
public:
    tcp_connection(asio::ip::tcp::socket&& socket);
    virtual ~tcp_connection() override;

    virtual void async_read(
            void* data,
            size_t length,
            const read_handler& handler) override;

    virtual void write(
            const void* data,
            size_t length,
            std::error_code& error_code) override;

private:
    asio::ip::tcp::socket m_socket;
};

inline tcp_connection::tcp_connection(asio::ip::tcp::socket&& socket)
    : rawsocket_connection()
    , m_socket(std::move(socket))
{
    // Disable Nagle algorithm to get lower latency (and lower throughput).
    m_socket.set_option(asio::ip::tcp::no_delay(true));
}

inline tcp_connection::~tcp_connection()
{
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

inline void tcp_connection::async_read(
        void* data,
        size_t length,
        const read_handler& handler)
{
    asio::async_read(m_socket, asio::buffer(data, length), handler);
}

inline void tcp_connection::write(
        const void* data,
        size_t length,
        std::error_code& error_code)
{
    asio::write(m_socket, asio::buffer(data, length), error_code);
}

} // namespace bonefish

#endif // BONEFISH_TCP_CONNECTION_HPP
