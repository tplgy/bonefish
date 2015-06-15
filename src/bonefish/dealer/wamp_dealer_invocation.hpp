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
    void set_timeout(timeout_callback callback, unsigned timeout_sec);

    const wamp_request_id& get_request_id() const;
    const std::weak_ptr<wamp_session>& get_session() const;

private:
    wamp_request_id m_request_id;

    // We use a weak pointer to the session here because there is no
    // convenient way to cleanup pending invocations when a sesson is
    // detached without walking the entire mapping of outstanding
    // invocations. So instead we allow things to simply timeout and
    // clean themselves up naturally.
    std::weak_ptr<wamp_session> m_session;
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

inline void wamp_dealer_invocation::set_timeout(timeout_callback callback, unsigned timeout_sec)
{
    m_timeout_timer.expires_from_now(boost::posix_time::seconds(timeout_sec));
    m_timeout_timer.async_wait(callback);
}

inline const std::weak_ptr<wamp_session>& wamp_dealer_invocation::get_session() const
{
    return m_session;
}

inline const wamp_request_id& wamp_dealer_invocation::get_request_id() const
{
    return m_request_id;
}

} // namespace bonefish

#endif // BONEFISH_DEALER_WAMP_DEALER_INVOCATION_HPP
