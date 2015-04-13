#ifndef BONEFISH_TCP_CONNECTION_HPP
#define BONEFISH_TCP_CONNECTION_HPP

#include <bonefish/common/wamp_connection_base.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>

namespace bonefish {

// NOTE: It appears that it is possible to templatize this
//       class based on the socket type which would allow
//       us to reuse it for unix domain sockets.
class tcp_connection :
        public rawsocket_connection,
        public std::enable_shared_from_this<tcp_connection>
{
public:
    tcp_connection(boost::asio::ip::tcp::socket&& socket);
    ~tcp_connection();

    virtual void async_handshake() override;
    virtual void async_receive() override;

    virtual void send_handshake(uint32_t capabilities) override;
    virtual void send_message(const char* message, size_t length) override;

private:
    void receive_handshake_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_header_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_body_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);

    void handle_system_error(const boost::system::error_code& error_code);

private:
    uint32_t m_capabilities;
    uint32_t m_message_length;
    std::vector<char> m_message_buffer;
    boost::asio::ip::tcp::socket m_socket;
};

} // namespace bonefish

#endif // BONEFISH_TCP_CONNECTION_HPP
