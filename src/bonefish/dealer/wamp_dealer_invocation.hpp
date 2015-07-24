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

#ifndef BONEFISH_DEALER_WAMP_DEALER_INVOCATION_HPP
#define BONEFISH_DEALER_WAMP_DEALER_INVOCATION_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/session/wamp_session.hpp>

#include <boost/asio.hpp>
#include <functional>
#include <unordered_set>

namespace bonefish {

class wamp_dealer_invocation
{
public:
    typedef std::function<void(const boost::system::error_code&)> timeout_callback;

public:
    wamp_dealer_invocation(boost::asio::io_service& io_service);
    ~wamp_dealer_invocation();

    void set_request_id(const wamp_request_id& request_id);
    void set_session(const std::shared_ptr<wamp_session>& session);
    void set_timeout(timeout_callback callback, unsigned timeout_ms);

    const wamp_request_id& get_request_id() const;
    const std::shared_ptr<wamp_session>& get_session() const;

private:
    wamp_request_id m_request_id;

    // We use a shared pointer to the session here because there is no
    // convenient way to cleanup pending invocations when a sesson is
    // detached without walking the entire mapping of outstanding
    // invocations. So instead we allow things to simply timeout and
    // clean themselves up naturally.
    std::shared_ptr<wamp_session> m_session;
    boost::asio::deadline_timer m_timeout_timer;
};

inline wamp_dealer_invocation::wamp_dealer_invocation(boost::asio::io_service& io_service)
    : m_request_id()
    , m_session()
    , m_timeout_timer(io_service)
{
}

inline wamp_dealer_invocation::~wamp_dealer_invocation()
{
    m_timeout_timer.cancel();
}

inline void wamp_dealer_invocation::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_dealer_invocation::set_session(const std::shared_ptr<wamp_session>& session)
{
    m_session = session;
}

inline void wamp_dealer_invocation::set_timeout(timeout_callback callback, unsigned timeout_ms)
{
    // Do not allow setting a timeout value of 0 as this is a
    // special timeout value indicating infinite timeout. So
    // insted we just don't arm the timer which gives us an
    // infinite timeout.
    if (timeout_ms) {
        m_timeout_timer.expires_from_now(boost::posix_time::milliseconds(timeout_ms));
        m_timeout_timer.async_wait(callback);
    }
}

inline const std::shared_ptr<wamp_session>& wamp_dealer_invocation::get_session() const
{
    return m_session;
}

inline const wamp_request_id& wamp_dealer_invocation::get_request_id() const
{
    return m_request_id;
}

} // namespace bonefish

#endif // BONEFISH_DEALER_WAMP_DEALER_INVOCATION_HPP
