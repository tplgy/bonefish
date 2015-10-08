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

#include <bonefish/native/native_server_impl.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>
#include <bonefish/native/native_component_endpoint.hpp>
#include <bonefish/native/native_connection.hpp>
#include <bonefish/native/native_connector.hpp>
#include <bonefish/native/native_server_endpoint.hpp>
#include <bonefish/native/native_transport.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/trace/trace.hpp>

#include <stdexcept>

namespace bonefish {

native_server_impl::native_server_impl(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers)
    : m_io_service(io_service)
    , m_routers(routers)
    , m_connector(std::make_shared<native_connector>())
    , m_connections()
    , m_endpoints_connected()
    , m_message_processor(routers)
{
    std::weak_ptr<native_server_impl> weak_this = shared_from_this();

    auto connect_handler = [this, weak_this](
            const std::shared_ptr<native_component_endpoint>& component_endpoint) {
        auto shared_this = weak_this.lock();
        if (!shared_this) {
            // Will be thrown in the context of the component
            throw std::runtime_error("connect failed");
        }
        auto server_endpoint = on_connect(component_endpoint);
        auto connected_handler = component_endpoint->get_connected_handler();
        connected_handler(server_endpoint);
    };
    m_connector->set_connect_handler(connect_handler);

    auto disconnect_handler = [this, weak_this](
            const std::shared_ptr<native_server_endpoint>& server_endpoint) {
        auto shared_this = weak_this.lock();
        if (shared_this) {
            // Will be thrown in the context of the component
            throw std::runtime_error("disconnect failed");
        }
        auto component_endpoint = on_disconnect(server_endpoint);
        auto disconnected_handler = component_endpoint->get_disconnected_handler();
        disconnected_handler();
    };
    m_connector->set_disconnect_handler(disconnect_handler);
}

native_server_impl::~native_server_impl()
{
}

void native_server_impl::start()
{
    BONEFISH_TRACE("starting native server");
}

void native_server_impl::shutdown()
{
    BONEFISH_TRACE("stopping native server");
    // FIXME: Walk all of the connections and disconnect them?
}

std::shared_ptr<native_server_endpoint> native_server_impl::on_connect(
        const std::shared_ptr<native_component_endpoint>& component_endpoint)
{
    auto connection = std::make_shared<native_connection>(
            m_io_service, component_endpoint);
    std::weak_ptr<native_server_impl> weak_this = shared_from_this();

    auto receive_handler = [this, weak_this](
            const std::shared_ptr<native_connection>& connection,
            std::vector<msgpack::object>&& fields,
            msgpack::zone&& zone) {
        auto shared_this = weak_this.lock();
        if (!shared_this) {
            // There is nothing to do here but silently dismiss this
            // error condition. If the server or connection has gone
            // away then the message being recevied is meaningless.
            return;
        }

        on_message(connection, std::move(fields), std::move(zone));
    };
    connection->set_receive_message_handler(
            std::bind(receive_handler, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    auto server_endpoint = connection->get_server_endpoint();
    auto disconnected_handler = [this, weak_this](
            const std::shared_ptr<native_server_endpoint>& server_endpoint) {
        auto shared_this = weak_this.lock();
        if (!shared_this) {
            // There is nothing to do here but silently dismiss this
            // error condition. The server has gone away which means
            // the message we are trying to deliver is meaningless.
            return;
        }
        on_disconnect(server_endpoint);
    };
    connection->set_disconnected_handler(std::bind(disconnected_handler, server_endpoint));

    m_connections.insert(connection);
    m_endpoints_connected[server_endpoint] = connection;

    return server_endpoint;
}

std::shared_ptr<native_component_endpoint> native_server_impl::on_disconnect(
        const std::shared_ptr<native_server_endpoint>& server_endpoint)
{
    auto itr = m_endpoints_connected.find(server_endpoint);
    if (itr == m_endpoints_connected.end()) {
        std::invalid_argument("server endpoint does not exist");
    }

    auto connection = itr->second;
    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }

    m_endpoints_connected.erase(itr);
    m_connections.erase(connection);

    return connection->get_component_endpoint();
}

void native_server_impl::on_message(
        const std::shared_ptr<native_connection>& connection,
        const std::vector<msgpack::object>& fields,
        msgpack::zone&& zone)
{
    try {
        if (fields.size() < 1) {
            throw std::runtime_error("invalid message");
        }

        auto type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
        std::unique_ptr<wamp_message> message(wamp_message_factory::create_message(type));
        if (!message) {
            throw std::runtime_error("message type not supported");
        }
        message->unmarshal(fields, std::move(zone));

        std::unique_ptr<wamp_transport> transport(new native_transport(connection));
        m_message_processor.process_message(message, std::move(transport), connection.get());
    } catch (const std::exception& e) {
        BONEFISH_TRACE("unhandled exception: %1%", e.what());
    }
}

} // namespace bonefish
