#include <bonefish/tcp/tcp_server_impl.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_router.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/tcp/tcp_connection.hpp>
#include <bonefish/tcp/tcp_listener.hpp>
#include <bonefish/tcp/tcp_transport.hpp>
#include <bonefish/transport/wamp_transport.hpp>

#include <boost/asio/ip/address.hpp>

namespace bonefish {

tcp_server_impl::tcp_server_impl(boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers,
        const std::shared_ptr<wamp_session_id_generator>& generator)
    : m_io_service(io_service)
    , m_listener()
    , m_routers(routers)
    , m_serializers(serializers)
    , m_message_processor(routers, generator)
{
}

tcp_server_impl::~tcp_server_impl()
{
}

void tcp_server_impl::start(const boost::asio::ip::address& ip_address, uint16_t port)
{
    assert(!m_listener);
    m_listener = std::make_shared<tcp_listener>(m_io_service, ip_address, port);
    m_listener->set_accept_handler(
            std::bind(&tcp_server_impl::on_connect, shared_from_this(), std::placeholders::_1));
    m_listener->start_listening();
}

void tcp_server_impl::shutdown()
{
    if (m_listener) {
        m_listener->stop_listening();
    }
}

void tcp_server_impl::on_connect(const std::shared_ptr<tcp_connection>& connection)
{
    std::cerr << "created connection" << std::endl;
    connection->set_close_handler(std::bind(&tcp_server_impl::on_close,
            shared_from_this(), std::placeholders::_1));
    connection->set_fail_handler(std::bind(&tcp_server_impl::on_fail,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    connection->set_message_handler(std::bind(&tcp_server_impl::on_message,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_connections.insert(connection);
}

void tcp_server_impl::on_close(const std::shared_ptr<tcp_connection>& connection)
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

void tcp_server_impl::on_fail(const std::shared_ptr<tcp_connection>& connection, const char* reason)
{
    std::cerr << "connection failed: " << reason << std::endl;
    m_connections.erase(connection);
}

void tcp_server_impl::on_message(const std::shared_ptr<tcp_connection>& connection,
        const char* buffer, size_t length)
{
    std::cerr << "received message" << std::endl;
    try {
        std::shared_ptr<wamp_serializer> serializer =
                m_serializers->get_serializer(wamp_serializer_type::MSGPACK);
        std::unique_ptr<wamp_message> message(
                serializer->deserialize(buffer, length));
        std::unique_ptr<wamp_transport> transport(
                new tcp_transport(serializer, connection));

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
