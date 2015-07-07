#ifndef BONEFISH_TCP_LISTENER_HPP
#define BONEFISH_TCP_LISTENER_HPP

#include <bonefish/rawsocket/rawsocket_listener.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>

namespace bonefish {

class tcp_connection;

class tcp_listener :
        public rawsocket_listener,
        public std::enable_shared_from_this<tcp_listener>
{
public:
    tcp_listener(
            boost::asio::io_service& io_service,
            const boost::asio::ip::address& ip_address,
            uint16_t port);

    virtual ~tcp_listener() override;
    virtual void start_listening() override;
    virtual void stop_listening() override;

private:
    void async_accept();
    void handle_accept(const boost::system::error_code &ec);

private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::acceptor m_acceptor;
};

} // namespace bonefish

#endif // BONEFISH_TCP_LISTENER_HPP
