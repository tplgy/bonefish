#include <bonefish/rawsocket/tcp_connection.hpp>
#include <bonefish/trace/trace.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <iostream>

namespace bonefish {

tcp_connection::tcp_connection(boost::asio::ip::tcp::socket&& socket)
    : rawsocket_connection()
    , m_capabilities(0)
    , m_message_length(0)
    , m_message_buffer()
    , m_socket(std::move(socket))
{
    // Disable Nagle algorithm to get lower latency (and lower throughput).
    m_socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

tcp_connection::~tcp_connection()
{
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void tcp_connection::async_handshake()
{
    auto buffer = boost::asio::buffer(&m_capabilities, sizeof(m_capabilities));
    auto handler = std::bind(&tcp_connection::receive_handshake_handler,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2);

    boost::asio::async_read(m_socket, buffer, handler);
}

void tcp_connection::async_receive()
{
    auto buffer = boost::asio::buffer(&m_message_length, sizeof(m_message_length));
    auto handler = std::bind(&tcp_connection::receive_message_header_handler,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2);

    // We cannot start receiving messages until the initial
    // handshake has allowed us to exchange capabilities.
    assert(m_capabilities != 0);

    boost::asio::async_read(m_socket, buffer, handler);
}

void tcp_connection::send_handshake(uint32_t capabilities)
{
    boost::system::error_code error_code;
    boost::asio::write(m_socket,
            boost::asio::buffer(&capabilities, sizeof(capabilities)), error_code);
    if (error_code) {
        return handle_system_error(error_code);
    }
}

void tcp_connection::send_message(const char* message, size_t length)
{
    boost::system::error_code error_code;

    // First write the message length prefix.
    uint32_t length_prefix = htonl(length);
    boost::asio::write(m_socket, boost::asio::buffer(&length_prefix, sizeof(length_prefix)), error_code);
    if (error_code) {
        return handle_system_error(error_code);
    }

    // Then write the actual message.
    boost::asio::write(m_socket, boost::asio::buffer(message, length), error_code);
    if (error_code) {
        return handle_system_error(error_code);
    }
}

void tcp_connection::receive_handshake_handler(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    assert(bytes_transferred == sizeof(m_capabilities));
    const auto& handshake_handler = get_handshake_handler();
    handshake_handler(shared_from_this(), ntohl(m_capabilities));
}

void tcp_connection::receive_message_header_handler(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    // We cannot be guaranteed that a client implementation won't accidentally
    // introduce this protocol violation. In the event that we ever encounter
    // a message that reports a zero length we fail that connection gracefully.
    assert(m_message_length != 0);
    if (m_message_length == 0) {
        BONEFISH_TRACE("invalid message length: %1%", m_message_length);
        const auto& fail_handler = get_fail_handler();
        fail_handler(shared_from_this(), "invalid message length");
    }

    m_message_buffer.reserve(ntohl(m_message_length));

    auto buffer = boost::asio::buffer(m_message_buffer.data(), ntohl(m_message_length));
    auto handler = std::bind(&tcp_connection::receive_message_body_handler,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2);

    boost::asio::async_read(m_socket, buffer, handler);
}

void tcp_connection::receive_message_body_handler(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    const auto& message_handler = get_message_handler();
    assert(message_handler);
    message_handler(shared_from_this(), m_message_buffer.data(), bytes_transferred);

    async_receive();
}

void tcp_connection::handle_system_error(const boost::system::error_code& error_code)
{
    // NOTE: The boost documentation does not indicate what all of the possible error
    //       codes are that can occur for the async receive handlers. So it will be an
    //       ongoing exercise in trying to figure this out.
    if (error_code == boost::asio::error::eof) {
        const auto& close_handler = get_close_handler();
        close_handler(shared_from_this());
    } else if (error_code != boost::asio::error::operation_aborted) {
        BONEFISH_TRACE("connection failed: %1%", error_code);
        const auto& fail_handler = get_fail_handler();
        fail_handler(shared_from_this(), error_code.message().c_str());
    }
}

} // namespace bonefish
