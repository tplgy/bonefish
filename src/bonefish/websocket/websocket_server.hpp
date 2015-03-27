#ifndef BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP
#define BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstdint>
#include <memory>

namespace bonefish {

class wamp_routers;
class wamp_serializers;
class wamp_session_id_generator;
class websocket_server_impl;

class websocket_server
{
public:
    websocket_server(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers,
            const std::shared_ptr<wamp_session_id_generator>& generator);
    ~websocket_server();

    void start(const boost::asio::ip::address& ip_address, uint16_t port);
    void shutdown();

private:
    std::unique_ptr<websocket_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_WEBSOCKET_SERVER_HPP
