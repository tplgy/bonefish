#ifndef BONEFISH_TCP_LISTENER_HPP
#define BONEFISH_TCP_LISTENER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <functional>

namespace bonefish {

class tcp_connection;

class tcp_listener : public std::enable_shared_from_this<tcp_listener>
{
public:
    using accept_handler = std::function<void(const std::shared_ptr<tcp_connection>&)>;

public:
    tcp_listener(boost::asio::io_service& io_service,
            const boost::asio::ip::address& ip_address,
            uint16_t port);
    ~tcp_listener();

    void set_accept_handler(const accept_handler& handler);
    void start_listening();
    void stop_listening();
    bool is_listening() const;

private:
    void async_accept();
    void handle_accept(const boost::system::error_code &ec);

private:
    bool m_listening;
    accept_handler m_accept_handler;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::acceptor m_acceptor;
};

} // namespace bonefish

#endif // BONEFISH_TCP_LISTENER_HPP
