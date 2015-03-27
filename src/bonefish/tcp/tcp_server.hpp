#ifndef BONEFISH_TCP_SERVER_HPP
#define BONEFISH_TCP_SERVER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class tcp_server_impl;
class wamp_routers;
class wamp_serializers;
class wamp_session_id_generator;

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~tcp_server();

    void start(const boost::asio::ip::address& ip_address, uint16_t port);
    void shutdown();

private:
    std::shared_ptr<tcp_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_TCP_SERVER_HPP
