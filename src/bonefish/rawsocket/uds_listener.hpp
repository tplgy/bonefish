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

#ifndef BONEFISH_UDS_LISTENER_HPP
#define BONEFISH_UDS_LISTENER_HPP

#include <bonefish/rawsocket/rawsocket_listener.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <memory>
#include <string>

namespace bonefish {

class uds_listener :
        public rawsocket_listener,
        public std::enable_shared_from_this<uds_listener>
{
public:
    uds_listener(
            boost::asio::io_service& io_service,
            const std::string& path);
    virtual ~uds_listener() override;

    virtual void start_listening() override;
    virtual void stop_listening() override;
    virtual std::shared_ptr<rawsocket_connection> create_connection() override;

protected:
    virtual void async_accept() override;

private:
    boost::asio::local::stream_protocol::socket m_socket;
    boost::asio::local::stream_protocol::acceptor m_acceptor;
    boost::asio::local::stream_protocol::endpoint m_endpoint;
};

} // namespace bonefish

#endif // BONEFISH_UDS_LISTENER_HPP
