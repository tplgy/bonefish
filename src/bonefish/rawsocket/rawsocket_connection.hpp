#ifndef BONEFISH_RAWSOCKET_CONNECTION_HPP
#define BONEFISH_RAWSOCKET_CONNECTION_HPP

#include <bonefish/common/wamp_connection_base.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <functional>

namespace bonefish {

class rawsocket_connection :
        public wamp_connection_base,
        public std::enable_shared_from_this<rawsocket_connection>
{
public:
    using close_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&)>;
    using fail_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&, const char*)>;
    using message_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&, const char*, size_t)>;
    using handshake_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&, uint32_t)>;

public:
    rawsocket_connection();
    virtual ~rawsocket_connection() override;

    virtual void async_handshake() = 0;
    virtual void async_receive() = 0;

    virtual bool send_handshake(uint32_t capabilities) = 0;
    virtual bool send_message(const char* message, size_t length) = 0;

    const close_handler& get_close_handler() const;
    const fail_handler& get_fail_handler() const;
    const message_handler& get_message_handler() const;
    const handshake_handler& get_handshake_handler() const;

    void set_close_handler(const close_handler& handler);
    void set_fail_handler(const fail_handler& handler);
    void set_message_handler(const message_handler& handler);
    void set_handshake_handler(const handshake_handler& handler);

private:
    close_handler m_close_handler;
    fail_handler m_fail_handler;
    message_handler m_message_handler;
    handshake_handler m_handshake_handler;
};

inline rawsocket_connection::rawsocket_connection()
    : wamp_connection_base()
    , m_close_handler()
    , m_fail_handler()
    , m_message_handler()
    , m_handshake_handler()
{
}

inline rawsocket_connection::~rawsocket_connection()
{
}

inline const rawsocket_connection::close_handler& rawsocket_connection::get_close_handler() const
{
    return m_close_handler;
}

inline const rawsocket_connection::fail_handler& rawsocket_connection::get_fail_handler() const
{
    return m_fail_handler;
}

inline const rawsocket_connection::message_handler& rawsocket_connection::get_message_handler() const
{
    return m_message_handler;
}

inline const rawsocket_connection::handshake_handler& rawsocket_connection::get_handshake_handler() const
{
    return m_handshake_handler;
}

inline void rawsocket_connection::set_close_handler(const rawsocket_connection::close_handler& handler)
{
    m_close_handler = handler;
}

inline void rawsocket_connection::set_fail_handler(const rawsocket_connection::fail_handler& handler)
{
    m_fail_handler = handler;
}

inline void rawsocket_connection::set_message_handler(const rawsocket_connection::message_handler& handler)
{
    m_message_handler = handler;
}

inline void rawsocket_connection::set_handshake_handler(const rawsocket_connection::handshake_handler& handler)
{
    m_handshake_handler = handler;
}

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_CONNECTION_HPP
