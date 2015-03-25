#include <bonefish/tcp/tcp_listener.hpp>
#include <bonefish/tcp/tcp_connection.hpp>

namespace bonefish {

tcp_listener::tcp_listener(boost::asio::io_service& io_service,
        const boost::asio::ip::address& ip_address,
        uint16_t port)
    : m_listening(false)
    , m_accept_handler(nullptr)
    , m_socket(io_service)
    , m_acceptor(io_service,
            boost::asio::ip::tcp::endpoint(ip_address, port))
{
}

tcp_listener::~tcp_listener()
{
    if (is_listening()) {
        stop_listening();
    }
}

void tcp_listener::set_accept_handler(const accept_handler& handler)
{
    m_accept_handler = handler;
}

void tcp_listener::start_listening()
{
    assert(m_accept_handler);
    m_listening = true;
    async_accept();
}

void tcp_listener::stop_listening()
{
    m_acceptor.cancel();
    m_listening = false;
}

bool tcp_listener::is_listening() const
{
    return m_listening;
}

void tcp_listener::handle_accept(const boost::system::error_code &error_code)
{
    if (error_code) {
        return;
    }

    assert(m_accept_handler);
    auto connection = std::make_shared<tcp_connection>(std::move(m_socket));
    m_accept_handler(connection);

    async_accept();
}

void tcp_listener::async_accept()
{
    m_acceptor.async_accept(m_socket,
            std::bind(&tcp_listener::handle_accept, shared_from_this(),
            std::placeholders::_1));
}

} // namespace bonefish
