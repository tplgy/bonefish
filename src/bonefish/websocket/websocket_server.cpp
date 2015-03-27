#include <bonefish/websocket/websocket_server.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/websocket/websocket_server_impl.hpp>

namespace bonefish {

websocket_server::websocket_server(
        boost::asio::io_service& io_service,
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_impl(new websocket_server_impl(io_service, routers, serializers))
{
}

websocket_server::~websocket_server()
{
}

void websocket_server::start(const boost::asio::ip::address& ip_address, uint16_t port)
{
    m_impl->start(ip_address, port);
}

void websocket_server::shutdown()
{
    m_impl->shutdown();
}

} // namespace bonefish
