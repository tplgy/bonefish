#ifndef BONEFISH_TCP_SERVER_HPP
#define BONEFISH_TCP_SERVER_HPP

#include <bonefish/common/wamp_message_processor.hpp>
#include <bonefish/tcp/tcp_listener.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class wamp_session_id_generator;
class tcp_connection;
class wamp_routers;
class wamp_serializers;

class tcp_server : public std::enable_shared_from_this<tcp_server>
{
public:
    tcp_server(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers,
            const std::shared_ptr<wamp_session_id_generator>& generator);
    ~tcp_server();

    void start(const boost::asio::ip::address& ip_address, uint16_t port);
    void shutdown();

private:
    void on_connect(const std::shared_ptr<tcp_connection>& connection);
    void on_close(const std::shared_ptr<tcp_connection>& connection);
    void on_fail(const std::shared_ptr<tcp_connection>& connection, const char* reason);
    void on_message(const std::shared_ptr<tcp_connection>& connection,
            const char* buffer, size_t length);

private:
    boost::asio::io_service& m_io_service;
    std::shared_ptr<tcp_listener> m_listener;
    std::shared_ptr<wamp_routers> m_routers;
    std::shared_ptr<wamp_serializers> m_serializers;

    std::set<std::shared_ptr<tcp_connection>,
            std::owner_less<std::shared_ptr<tcp_connection>>> m_connections;

    wamp_message_processor m_message_processor;
};

} // namespace bonefish

#endif // BONEFISH_TCP_SERVER_HPP
