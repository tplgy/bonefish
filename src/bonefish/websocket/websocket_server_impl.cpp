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

#include <bonefish/websocket/websocket_server_impl.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/trace/trace.hpp>
#include <bonefish/websocket/websocket_protocol.hpp>
#include <bonefish/websocket/websocket_transport.hpp>

#include <boost/asio/io_service.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/utilities.hpp>

namespace bonefish {

websocket_server_impl::websocket_server_impl(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_io_service(io_service)
    , m_server(new websocketpp::server<websocket_config>())
    , m_routers(routers)
    , m_serializers(serializers)
    , m_message_processor(m_routers)
{
}

websocket_server_impl::~websocket_server_impl()
{
}

void websocket_server_impl::start(const boost::asio::ip::address& ip_address, uint16_t port)
{
    BONEFISH_TRACE("starting websocket server: %1%:%2%", ip_address.to_string() % port);

    boost::asio::ip::tcp::endpoint endpoint(ip_address, port);

    std::weak_ptr<websocket_server_impl> weak_self = shared_from_this();

    auto init_handler = [weak_self](
            websocketpp::connection_hdl handle,
            boost::asio::ip::tcp::socket& socket) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_socket_init(handle, socket);
        }
    };
    m_server->set_socket_init_handler(init_handler);

    auto open_handler = [weak_self](websocketpp::connection_hdl handle) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_open(handle);
        }
    };
    m_server->set_open_handler(open_handler);

    auto close_handler = [weak_self](websocketpp::connection_hdl handle) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_close(handle);
        }
    };
    m_server->set_close_handler(close_handler);

    auto fail_handler = [weak_self](websocketpp::connection_hdl handle) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_fail(handle);
        }
    };
    m_server->set_fail_handler(fail_handler);

    auto validate_handler = [weak_self](websocketpp::connection_hdl handle) -> bool {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            return shared_self->on_validate(handle);
        }
        return false;
    };
    m_server->set_validate_handler(validate_handler);

    auto message_handler = [weak_self](
            websocketpp::connection_hdl handle,
            websocketpp::server<websocket_config>::message_ptr message) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->on_message(handle, message);
        }
    };
    m_server->set_message_handler(message_handler);

    // Set log settings
    if (bonefish::trace::is_enabled()) {
        m_server->set_access_channels(websocketpp::log::alevel::all);
        m_server->set_error_channels(websocketpp::log::elevel::all);
    } else {
        m_server->set_access_channels(websocketpp::log::alevel::none);
        m_server->set_error_channels(websocketpp::log::elevel::none);
    }

    m_server->init_asio(&m_io_service);
    m_server->set_reuse_addr(true);
    m_server->listen(endpoint);
    m_server->start_accept();
}

void websocket_server_impl::shutdown()
{
    BONEFISH_TRACE("stopping websocket server");
    m_server->stop_listening();
}

void websocket_server_impl::on_socket_init(websocketpp::connection_hdl handle,
        boost::asio::ip::tcp::socket& s)
{
    s.set_option(boost::asio::ip::tcp::no_delay(true));
}

void websocket_server_impl::on_open(websocketpp::connection_hdl handle)
{
}

void websocket_server_impl::on_close(websocketpp::connection_hdl handle)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }
}

void websocket_server_impl::on_fail(websocketpp::connection_hdl handle)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }
}

bool websocket_server_impl::on_validate(websocketpp::connection_hdl handle)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);

    const auto& subprotocols = connection->get_requested_subprotocols();
    for (const auto& subprotocol : subprotocols) {
        if (subprotocol == WAMPV2_MSGPACK_SUBPROTOCOL) {
            if (m_serializers->has_serializer(wamp_serializer_type::MSGPACK)) {
                connection->select_subprotocol(subprotocol);
                return true;
            }
        }

        if (subprotocol == WAMPV2_JSON_SUBPROTOCOL) {
            if (m_serializers->has_serializer(wamp_serializer_type::JSON)) {
                connection->select_subprotocol(subprotocol);
                return true;
            }
        }
    }

    BONEFISH_TRACE("no supported subprotocol found ... rejecting connection");
    return false;
}

void websocket_server_impl::on_message(websocketpp::connection_hdl handle,
        websocketpp::server<websocket_config>::message_ptr buffer)
{
    websocketpp::server<websocket_config>::connection_ptr connection =
            m_server->get_con_from_hdl(handle);
    std::shared_ptr<wamp_serializer> serializer;

    if (connection->get_subprotocol() == WAMPV2_MSGPACK_SUBPROTOCOL) {
        serializer = m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
    } else if (connection->get_subprotocol() == WAMPV2_JSON_SUBPROTOCOL) {
        serializer = m_serializers->get_serializer(wamp_serializer_type::JSON);
    }

    try {
        std::unique_ptr<wamp_message> message(
                serializer->deserialize(buffer->get_payload().c_str(), buffer->get_payload().size()));
        std::unique_ptr<wamp_transport> transport(
                new websocket_transport(serializer, handle, m_server));

        if (message) {
            m_message_processor.process_message(message, std::move(transport), connection.get());
        }
    } catch (const std::exception& e) {
        BONEFISH_TRACE("unhandled exception: %1%", e.what());
    }
}

} // namespace bonefish
