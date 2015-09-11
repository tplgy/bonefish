/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef BONEFISH_RAWSOCKET_CONNECTION_HPP
#define BONEFISH_RAWSOCKET_CONNECTION_HPP

#include <bonefish/common/wamp_connection_base.hpp>
#include <bonefish/trace/trace.hpp>

#include <arpa/inet.h>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>
#include <cstdint>
#include <functional>
#include <memory>

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
    using read_handler = std::function<void(const boost::system::error_code&, size_t)>;

public:
    rawsocket_connection();
    virtual ~rawsocket_connection() override;

    virtual void async_read(
            void* data,
            size_t length,
            const read_handler& handler) = 0;

    virtual void write(
            const void* data,
            size_t length,
            boost::system::error_code& error_code) = 0;

    void async_handshake();
    void async_receive();

    bool send_handshake(uint32_t capabilities);
    bool send_message(const char* message, size_t length);

    const close_handler& get_close_handler() const;
    const fail_handler& get_fail_handler() const;
    const message_handler& get_message_handler() const;
    const handshake_handler& get_handshake_handler() const;

    void set_close_handler(const close_handler& handler);
    void set_fail_handler(const fail_handler& handler);
    void set_message_handler(const message_handler& handler);
    void set_handshake_handler(const handshake_handler& handler);

private:
    void receive_handshake_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_header_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);
    void receive_message_body_handler(
            const boost::system::error_code& error_code, size_t bytes_transferred);

    void handle_system_error(const boost::system::error_code& error_code);

private:
    close_handler m_close_handler;
    fail_handler m_fail_handler;
    message_handler m_message_handler;
    handshake_handler m_handshake_handler;

    uint32_t m_capabilities;
    uint32_t m_message_length; // stored in network order
    std::vector<char> m_message_buffer;
};

inline rawsocket_connection::rawsocket_connection()
    : wamp_connection_base()
    , m_close_handler()
    , m_fail_handler()
    , m_message_handler()
    , m_handshake_handler()
    , m_capabilities(0)
    , m_message_length(0)
    , m_message_buffer()
{
}

inline rawsocket_connection::~rawsocket_connection()
{
}

inline void rawsocket_connection::async_handshake()
{
    std::weak_ptr<rawsocket_connection> weak_self =
            std::static_pointer_cast<rawsocket_connection>(shared_from_this());

    auto handler = [weak_self](
            const boost::system::error_code& error_code, size_t bytes_transferred) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->receive_handshake_handler(error_code, bytes_transferred);
        }
    };

    async_read(&m_capabilities, sizeof(m_capabilities), handler);
}

inline void rawsocket_connection::async_receive()
{
    std::weak_ptr<rawsocket_connection> weak_self =
            std::static_pointer_cast<rawsocket_connection>(shared_from_this());

    auto handler = [weak_self](
            const boost::system::error_code& error_code, size_t bytes_transferred) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->receive_message_header_handler(error_code, bytes_transferred);
        }
    };

    // We cannot start receiving messages until the initial
    // handshake has allowed us to exchange capabilities.
    assert(m_capabilities != 0);

    async_read(&m_message_length, sizeof(m_message_length), handler);
}

inline bool rawsocket_connection::send_handshake(uint32_t capabilities)
{
    boost::system::error_code error_code;
    write(&capabilities, sizeof(capabilities), error_code);

    if (error_code) {
        handle_system_error(error_code);
        return false;
    }

    return true;
}

inline bool rawsocket_connection::send_message(const char* message, size_t length)
{
    boost::system::error_code error_code;

    // First write the message length prefix.
    uint32_t length_prefix = htonl(length);
    write(&length_prefix, sizeof(length_prefix), error_code);
    if (error_code) {
        handle_system_error(error_code);
        return false;
    }

    // Then write the actual message.
    write(message, length, error_code);
    if (error_code) {
        handle_system_error(error_code);
        return false;
    }

    return true;
}

inline const typename rawsocket_connection::close_handler&
rawsocket_connection::get_close_handler() const
{
    return m_close_handler;
}

inline const typename rawsocket_connection::fail_handler&
rawsocket_connection::get_fail_handler() const
{
    return m_fail_handler;
}

inline const typename rawsocket_connection::message_handler&
rawsocket_connection::get_message_handler() const
{
    return m_message_handler;
}

inline const typename rawsocket_connection::handshake_handler&
rawsocket_connection::get_handshake_handler() const
{
    return m_handshake_handler;
}

inline void rawsocket_connection::set_close_handler(
        const rawsocket_connection::close_handler& handler)
{
    m_close_handler = handler;
}

inline void rawsocket_connection::set_fail_handler(
        const rawsocket_connection::fail_handler& handler)
{
    m_fail_handler = handler;
}

inline void rawsocket_connection::set_message_handler(
        const rawsocket_connection::message_handler& handler)
{
    m_message_handler = handler;
}

inline void rawsocket_connection::set_handshake_handler(
        const rawsocket_connection::handshake_handler& handler)
{
    m_handshake_handler = handler;
}

inline void rawsocket_connection::receive_handshake_handler(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    assert(bytes_transferred == sizeof(m_capabilities));
    const auto& handshake_handler = get_handshake_handler();
    handshake_handler(shared_from_this(), ntohl(m_capabilities));
}

inline void rawsocket_connection::receive_message_header_handler(
        const boost::system::error_code& error_code, size_t bytes_transferred)
{
    if (error_code) {
        return handle_system_error(error_code);
    }

    // We cannot be guaranteed that a client implementation won't accidentally
    // introduce this protocol violation. In the event that we ever encounter
    // a message that reports a zero length we fail that connection gracefully.
    static const uint32_t MAX_MESSAGE_LENGTH = 16*1024*1024; // 16MB
    if (m_message_length == 0 || ntohl(m_message_length) > MAX_MESSAGE_LENGTH) {
        BONEFISH_TRACE("invalid message length: %1%", m_message_length);
        const auto& fail_handler = get_fail_handler();
        return fail_handler(shared_from_this(), "invalid message length");
    }

    m_message_buffer.reserve(ntohl(m_message_length));

    std::weak_ptr<rawsocket_connection> weak_self =
            std::static_pointer_cast<rawsocket_connection>(shared_from_this());

    auto handler = [weak_self](
            const boost::system::error_code& error_code, size_t bytes_transferred) {
        auto shared_self = weak_self.lock();
        if (shared_self) {
            shared_self->receive_message_body_handler(error_code, bytes_transferred);
        }
    };

    async_read(m_message_buffer.data(), ntohl(m_message_length), handler);
}

inline void rawsocket_connection::receive_message_body_handler(
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

inline void rawsocket_connection::handle_system_error(const boost::system::error_code& error_code)
{
    // NOTE: The boost documentation does not indicate what all of the possible error
    //       codes are that can occur for the async receive handlers. So it will be an
    //       ongoing exercise in trying to figure this out.
    if (error_code == boost::asio::error::eof) {
        BONEFISH_TRACE("connection closed: %1%", error_code);
        const auto& close_handler = get_close_handler();
        close_handler(shared_from_this());
    } else if (error_code != boost::asio::error::operation_aborted) {
        BONEFISH_TRACE("connection failed: %1%", error_code);
        const auto& fail_handler = get_fail_handler();
        fail_handler(shared_from_this(), error_code.message().c_str());
    } else {
        BONEFISH_TRACE("unhandled system error: %1%", error_code);
        assert(0);
    }
}

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_CONNECTION_HPP
