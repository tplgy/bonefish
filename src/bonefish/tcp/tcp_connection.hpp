#ifndef BONEFISH_TCP_CONNECTION_HPP
#define BONEFISH_TCP_CONNECTION_HPP

#include <bonefish/common/wamp_connection_base.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>

namespace bonefish {

class tcp_connection :
        public wamp_connection_base,
        public std::enable_shared_from_this<tcp_connection>
{
public:
    using close_handler = std::function<void(const std::shared_ptr<tcp_connection>&)>;
    using fail_handler = std::function<void(const std::shared_ptr<tcp_connection>&, const char*)>;
    using message_handler = std::function<void(const std::shared_ptr<tcp_connection>&, const char*, size_t)>;

public:
    tcp_connection(boost::asio::ip::tcp::socket&& socket);
    ~tcp_connection();

    void set_close_handler(const close_handler& handler);
    void set_fail_handler(const fail_handler& handler);
    void set_message_handler(const message_handler& handler);
    void send_message(const char* message, size_t length);

private:
    void async_receive_message();
    void async_receive_message_header(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void async_receive_message_body(
            const boost::system::error_code& error_code, size_t bytes_transferred);

    void handle_system_error(const boost::system::error_code& error_code);

private:
    close_handler m_close_handler;
    fail_handler m_fail_handler;
    message_handler m_message_handler;

    static const uint32_t MESSAGE_HEADER_LENGTH = 4;
    char m_message_header_buffer[MESSAGE_HEADER_LENGTH];

    std::vector<char> m_message_buffer;
    boost::asio::ip::tcp::socket m_socket;
};

inline void tcp_connection::set_close_handler(const close_handler& handler)
{
    m_close_handler = handler;
}

inline void tcp_connection::set_fail_handler(const fail_handler& handler)
{
    m_fail_handler = handler;
}

inline void tcp_connection::set_message_handler(const message_handler& handler)
{
    m_message_handler = handler;
}

} // namespace bonefish

#endif // BONEFISH_TCP_CONNECTION_HPP
