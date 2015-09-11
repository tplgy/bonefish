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

#ifndef BONEFISH_UDS_CONNECTION_HPP
#define BONEFISH_UDS_CONNECTION_HPP

#include <bonefish/rawsocket/rawsocket_connection.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

namespace bonefish {

class uds_connection : public rawsocket_connection
{
public:
    uds_connection(boost::asio::local::stream_protocol::socket&& socket);
    virtual ~uds_connection() override;

    virtual void async_read(
            void* data,
            size_t length,
            const read_handler& handler) override;

    virtual void write(
            const void* data,
            size_t length,
            boost::system::error_code& error_code) override;

private:
    boost::asio::local::stream_protocol::socket m_socket;
};

inline uds_connection::uds_connection(boost::asio::local::stream_protocol::socket&& socket)
    : rawsocket_connection()
    , m_socket(std::move(socket))
{
    std::cerr << "creating uds connection" << std::endl;
}

inline uds_connection::~uds_connection()
{
    std::cerr << "destroying uds connection" << std::endl;
    if (m_socket.is_open()) {
        std::cerr << "Closed the uds socket" << std::endl;
        m_socket.close();
    }
}

inline void uds_connection::async_read(
        void* data,
        size_t length,
        const read_handler& handler)
{
    boost::asio::async_read(m_socket, boost::asio::buffer(data, length), handler);
}

inline void uds_connection::write(
        const void* data,
        size_t length,
        boost::system::error_code& error_code)
{
    boost::asio::write(m_socket, boost::asio::buffer(data, length), error_code);
}

} // namespace bonefish

#endif // BONEFISH_UDS_CONNECTION_HPP
