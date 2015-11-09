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

#ifndef BONEFISH_RAWSOCKET_LISTENER_HPP
#define BONEFISH_RAWSOCKET_LISTENER_HPP

#include <boost/asio/io_service.hpp>
#include <functional>

namespace bonefish {

class rawsocket_connection;

class rawsocket_listener
{
public:
    using error_handler = std::function<void(
            rawsocket_listener& listener,
            const boost::system::error_code& error_code)>;

    using accept_handler = std::function<void(const std::shared_ptr<rawsocket_connection>&)>;

public:
    rawsocket_listener();
    virtual ~rawsocket_listener();

    virtual void start_listening() = 0;
    virtual void stop_listening() = 0;
    virtual std::shared_ptr<rawsocket_connection> create_connection() = 0;

    bool is_listening() const;

    const error_handler& get_error_handler() const;
    void set_error_handler(const error_handler& handler);

    const accept_handler& get_accept_handler() const;
    void set_accept_handler(const accept_handler& handler);

    void handle_accept(const boost::system::error_code& error_code);

protected:
    virtual void async_accept() = 0;

    void set_listening(bool is_listening);

private:
    bool m_listening;
    error_handler m_error_handler;
    accept_handler m_accept_handler;
};

inline rawsocket_listener::rawsocket_listener()
    : m_listening(false)
    , m_accept_handler()
{
}

inline rawsocket_listener::~rawsocket_listener()
{
}

inline bool rawsocket_listener::is_listening() const
{
    return m_listening;
}

inline const typename rawsocket_listener::error_handler&
rawsocket_listener::get_error_handler() const
{
    return m_error_handler;
}

inline void rawsocket_listener::set_error_handler(
        const rawsocket_listener::error_handler& handler)
{
    m_error_handler = handler;
}

inline const typename rawsocket_listener::accept_handler&
rawsocket_listener::get_accept_handler() const
{
    return m_accept_handler;
}

inline void rawsocket_listener::set_accept_handler(
        const rawsocket_listener::accept_handler& handler)
{
    m_accept_handler = handler;
}

inline void rawsocket_listener::handle_accept(
        const boost::system::error_code &error_code)
{
    // If an error has occured, notify the registered error
    // handler so appropriate action can be taken.
    if (error_code) {
        assert(get_error_handler());
        const auto& error_handler = get_error_handler();
        error_handler(*this, error_code);
        return;
    }

    // Otherwise, this is a new incoming connection so go ahead
    // and create a new connection and notify the registered
    // accept handler.
    assert(get_accept_handler());
    const auto& accept_handler = get_accept_handler();
    accept_handler(create_connection());
    async_accept();
}

inline void rawsocket_listener::set_listening(bool is_listening)
{
    m_listening = is_listening;
}

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_LISTENER_HPP
