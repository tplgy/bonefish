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

#include <bonefish/common/wamp_connection_base.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>

namespace bonefish {

// NOTE: It appears that it is possible to templatize this
//       class based on the socket type which would allow
//       us to reuse it for unix domain sockets.
class tcp_connection :
        public rawsocket_connection
{
public:
    tcp_connection(boost::asio::ip::tcp::socket&& socket);
    ~tcp_connection();

    virtual void async_handshake() override;
    virtual void async_receive() override;

    virtual bool send_handshake(uint32_t capabilities) override;
    virtual bool send_message(const char* message, size_t length) override;

private:
    void receive_handshake_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_header_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_body_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);

    void handle_system_error(const boost::system::error_code& error_code);

private:
    uint32_t m_capabilities;
    uint32_t m_message_length;
    std::vector<char> m_message_buffer;
    boost::asio::ip::tcp::socket m_socket;
};

} // namespace bonefish

#endif // BONEFISH_TCP_CONNECTION_HPP
