#include <bonefish/tcp/tcp_connection.hpp>

#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <iostream>

namespace bonefish {

tcp_connection::tcp_connection(boost::asio::ip::tcp::socket&& socket)
    : m_close_handler()
    , m_fail_handler()
    , m_message_handler()
    , m_message_buffer()
    , m_socket(std::move(socket))
{
}

tcp_connection::~tcp_connection()
{
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void tcp_connection::send_message(const char* message, size_t length)
{
    boost::system::error_code error_code;
    boost::asio::write(m_socket, boost::asio::buffer(message, length), error_code);

    if (error_code) {
        return handle_system_error(error_code);
    }
}

void tcp_connection::async_receive_message()
{
    auto buffer = boost::asio::buffer(m_message_header_buffer, MESSAGE_HEADER_LENGTH);
    auto handler = std::bind(&tcp_connection::async_receive_message_header,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2);

    boost::asio::async_read(m_socket, buffer, handler);
}

void tcp_connection::async_receive_message_header(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    size_t message_length = ntohl(*reinterpret_cast<uint32_t*>(m_message_header_buffer));
    m_message_buffer.reserve(message_length);

    auto buffer = boost::asio::buffer(m_message_buffer.data(), message_length);
    auto handler = std::bind(&tcp_connection::async_receive_message_body,
            shared_from_this(), std::placeholders::_1, std::placeholders::_2);

    boost::asio::async_read(m_socket, buffer, handler);
}

void tcp_connection::async_receive_message_body(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    assert(m_message_handler);
    m_message_handler(shared_from_this(), m_message_buffer.data(), bytes_transferred);
}

void tcp_connection::handle_system_error(const boost::system::error_code& error_code)
{
    // NOTE: The boost documentation does not indicate what all of the possible error
    //       codes are that can occur for the async receive handlers. So it will be an
    //       ongoing exercise in trying to figure this out.
    if (error_code == boost::asio::error::eof) {
        assert(m_close_handler);
        std::cerr << "connection is closed" << std::endl;
        m_close_handler(shared_from_this());
    } else if (error_code != boost::asio::error::operation_aborted) {
        assert(m_fail_handler);
        std::cerr << "connection failed: " << error_code << std::endl;
        m_fail_handler(shared_from_this(), error_code.message().c_str());
    }
}

} // namespace bonefish
