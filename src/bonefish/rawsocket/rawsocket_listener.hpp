#ifndef BONEFISH_RAWSOCKET_LISTENER_HPP
#define BONEFISH_RAWSOCKET_LISTENER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <functional>

namespace bonefish {

class rawsocket_connection;

class rawsocket_listener
{
public:
    using accept_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&)>;

public:
    rawsocket_listener();
    virtual ~rawsocket_listener();

    virtual void start_listening() = 0;
    virtual void stop_listening() = 0;

    bool is_listening() const;
    const accept_handler& get_accept_handler() const;

    void set_listening(bool is_listening);
    void set_accept_handler(const accept_handler& handler);

private:
    bool m_listening;
    accept_handler m_accept_handler;
};

inline rawsocket_listener::rawsocket_listener()
    : m_accept_handler()
{
}

inline rawsocket_listener::~rawsocket_listener()
{
}

inline bool rawsocket_listener::is_listening() const
{
    return m_listening;
}

inline const rawsocket_listener::accept_handler& rawsocket_listener::get_accept_handler() const
{
    return m_accept_handler;
}

inline void rawsocket_listener::set_listening(bool is_listening)
{
    m_listening = is_listening;
}

inline void rawsocket_listener::set_accept_handler(const rawsocket_listener::accept_handler& handler)
{
    m_accept_handler = handler;
}

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_LISTENER_HPP
