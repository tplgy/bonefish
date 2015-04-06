#include <bonefish/rawsocket/rawsocket_server_impl.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/rawsocket/rawsocket_listener.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>
#include <bonefish/rawsocket/rawsocket_transport.hpp>
#include <bonefish/transport/wamp_transport.hpp>

#include <boost/asio/ip/address.hpp>

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
    listener->set_accept_handler(
            std::bind(&rawsocket_server_impl::on_connect, shared_from_this(), std::placeholders::_1));
    m_listeners.insert(listener);
}

void rawsocket_server_impl::start()
{
    assert(!m_listeners.empty());
    for (auto& listener : m_listeners) {
        listener->start_listening();
    }
}

void rawsocket_server_impl::shutdown()
{
    for (auto& listener : m_listeners) {
        listener->stop_listening();
    }
}

void rawsocket_server_impl::on_connect(const std::shared_ptr<rawsocket_connection>& connection)
{
    std::cerr << "created connection" << std::endl;
    connection->set_close_handler(std::bind(&rawsocket_server_impl::on_close,
            shared_from_this(), std::placeholders::_1));
    connection->set_fail_handler(std::bind(&rawsocket_server_impl::on_fail,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    connection->set_message_handler(std::bind(&rawsocket_server_impl::on_message,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_connections.insert(connection);
}

void rawsocket_server_impl::on_close(const std::shared_ptr<rawsocket_connection>& connection)
{
    std::cerr << "connection closed" << std::endl;
    if (connection->has_session_id()) {
        std::shared_ptr<wamp_router> router =
                m_routers->get_router(connection->get_realm());
        if (router) {
            router->detach_session(connection->get_session_id());
        }
    }

    m_connections.erase(connection);
}

void rawsocket_server_impl::on_fail(const std::shared_ptr<rawsocket_connection>& connection, const char* reason)
{
    std::cerr << "connection failed: " << reason << std::endl;
    m_connections.erase(connection);
}

void rawsocket_server_impl::on_message(const std::shared_ptr<rawsocket_connection>& connection,
        const char* buffer, size_t length)
{
    std::cerr << "received message" << std::endl;
    try {
        std::shared_ptr<wamp_serializer> serializer =
                m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
        std::unique_ptr<wamp_message> message(
                serializer->deserialize(buffer, length));
        std::unique_ptr<wamp_transport> transport(
                new rawsocket_transport(serializer, connection));

        if (message) {
            m_message_processor.process_message(message, std::move(transport), connection.get());
        } else {
            std::cerr << "error: unable to deserialize message" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "unhandled exception: " << e.what() << std::endl;
    }
}

} // namespace bonefish
