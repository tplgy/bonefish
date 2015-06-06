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

#include <bonefish/rawsocket/rawsocket_server_impl.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/rawsocket/rawsocket_listener.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>
#include <bonefish/rawsocket/rawsocket_transport.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/trace/trace.hpp>

#include <boost/asio/ip/address.hpp>
#include <ios>

namespace bonefish {

rawsocket_server_impl::rawsocket_server_impl(
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_routers(routers)
    , m_serializers(serializers)
    , m_listeners()
    , m_connections()
    , m_message_processor(routers)
{
}

rawsocket_server_impl::~rawsocket_server_impl()
{
}

void rawsocket_server_impl::attach_listener(const std::shared_ptr<rawsocket_listener>& listener)
{
    std::weak_ptr<rawsocket_server_impl> weak_self = shared_from_this();
    listener->set_accept_handler([weak_self](const std::shared_ptr<rawsocket_connection>& connection) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_connect(connection);
        }
    });

    m_listeners.insert(listener);
}

void rawsocket_server_impl::start()
{
    BONEFISH_TRACE("starting rawsocket server");
    assert(!m_listeners.empty());
    for (auto& listener : m_listeners) {
        listener->start_listening();
    }
}

void rawsocket_server_impl::shutdown()
{
    BONEFISH_TRACE("stopping rawsocket server");
    for (auto& listener : m_listeners) {
        listener->stop_listening();
    }
}

void rawsocket_server_impl::on_connect(const std::shared_ptr<rawsocket_connection>& connection)
{
    std::weak_ptr<rawsocket_server_impl> weak_self = shared_from_this();

    auto handshake_handler = [weak_self](
            const std::shared_ptr<rawsocket_connection>& connection,
            uint32_t capabilities) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_handshake(connection, capabilities);
        }
    };
    connection->set_handshake_handler(handshake_handler);

    auto message_handler = [weak_self](
            const std::shared_ptr<rawsocket_connection>& connection,
            const char* buffer,
            size_t length) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_message(connection, buffer, length);
        }
    };
    connection->set_message_handler(message_handler);

    auto close_handler = [weak_self](const std::shared_ptr<rawsocket_connection>& connection) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_close(connection);
        }
    };
    connection->set_close_handler(close_handler);

    auto fail_handler = [weak_self](
            const std::shared_ptr<rawsocket_connection>& connection,
            const char* reason) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_fail(connection, reason);
        }
    };
    connection->set_fail_handler(fail_handler);

    // Prepare the connection to receive the asynchronous handshake that is
    // initiated by the client. The handshake handler will be called when
    // the handshake arrives.
    connection->async_handshake();
    m_connections.insert(connection);
}

void rawsocket_server_impl::on_close(const std::shared_ptr<rawsocket_connection>& connection)
{
    teardown_connection(connection);
}

void rawsocket_server_impl::on_fail(
        const std::shared_ptr<rawsocket_connection>& connection, const char* reason)
{
    teardown_connection(connection);
}

// Capabilities Format
//
// L - length bits
// S - serializer bits
// R - reserved bits (must be zeros)
//
// MSB                                 LSB
// 31                                    0
// 0111 1111 LLLL SSSS RRRR RRRR RRRR RRRR
void rawsocket_server_impl::on_handshake(
        const std::shared_ptr<rawsocket_connection>& connection, uint32_t capabilities)
{
    // If the first octet is not the magic number 0x7F value then
    // this is not a valid capabilities exchange and we should fail
    // the connection. See the advanced specification for details on
    // the choice of this magic number.
    uint32_t magic = (capabilities & 0xFF000000) >> 24;
    if (magic != 0x7F) {
        BONEFISH_TRACE("invalid capabilities: %1%", capabilities);
        return teardown_connection(connection);
    }

    // TODO: Ignore the length for now. We will need to add this in at some point
    //       but currently the autobahn client ignores it anyways. It will need
    //       to get cached in the connection properties.
    //uint32_t exponent = ((capabilities & 0x00F00000) >> 20) + 9;
    //uint32_t max_message_length = 1 << exponent;

    // Currently we only support msgpack serialization. If this changes we will
    // need to make sure that the requested serialization protocol is cached in
    // the connection properties so that the correct serializer can be associated
    // with the connection for message processing.
    uint32_t serializer = (capabilities & 0x000F0000) >> 16;
    if (serializer != 0x2) {
        BONEFISH_TRACE("invalid serializer specified: %1%", serializer);
        if (!connection->send_handshake(htonl(0x7F100000))) {
            BONEFISH_TRACE("failed to send handshake response to component: network failure");
            teardown_connection(connection);
        }
        return;
    }

    // Make sure that the reserved bits are all zeros as expected otherwise
    // send an error response back in the returned handshake.
    uint32_t reserved = capabilities & 0x0000FFFF;
    if (reserved != 0) {
        if (!connection->send_handshake(htonl(0x7F300000))) {
            BONEFISH_TRACE("failed to send handshake response to component: network failure");
            teardown_connection(connection);
        }
        return;
    }

    // TODO: For now just echo back the clients capabilities. Once we
    //       decide to support a maximum message length we will have
    //       to report that back to the client here.
    if (connection->send_handshake(htonl(capabilities))) {
        // Prepare the connection to start receiving wamp messages. We only have to
        // initiate this once and it will then continue to re-arm itself after each
        // message is received. This is effectively what switches us from a handshake
        // receiving state to a message receiving state.
        connection->async_receive();
    } else {
        BONEFISH_TRACE("failed to send handshake response to component: network failure");
        teardown_connection(connection);
    }
}

void rawsocket_server_impl::on_message(
        const std::shared_ptr<rawsocket_connection>& connection, const char* buffer, size_t length)
{
    try {
        std::shared_ptr<wamp_serializer> serializer =
                m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
        std::unique_ptr<wamp_message> message(
                serializer->deserialize(buffer, length));
        std::unique_ptr<wamp_transport> transport(
                new rawsocket_transport(serializer, connection));

        if (message) {
            m_message_processor.process_message(message, std::move(transport), connection.get());
        }
    } catch (const std::exception& e) {
        BONEFISH_TRACE("unhandled exception: %1%", e.what());
    }
}

void rawsocket_server_impl::teardown_connection(
        const std::shared_ptr<rawsocket_connection>& connection)
{
    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }

    m_connections.erase(connection);
}

} // namespace bonefish
