#ifndef BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP
#define BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP

#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <unordered_set>

namespace bonefish {

class wamp_dealer_registration
{
public:
    wamp_dealer_registration();
    wamp_dealer_registration(const std::shared_ptr<wamp_session>& session,
            const wamp_registration_id& registration_id);
    ~wamp_dealer_registration();

    void set_session(const std::shared_ptr<wamp_session>& session);
    void set_registration_id(const wamp_registration_id& registration_id);

    const wamp_registration_id& get_registration_id() const;
    const std::shared_ptr<wamp_session>& get_session() const;

private:
    std::shared_ptr<wamp_session> m_session;
    wamp_registration_id m_registration_id;
};

inline wamp_dealer_registration::wamp_dealer_registration()
    : m_session()
    , m_registration_id()
{
}

inline wamp_dealer_registration::wamp_dealer_registration(const std::shared_ptr<wamp_session>& session,
        const wamp_registration_id& registration_id)
    : m_session(session)
    , m_registration_id(registration_id)
{
}

inline wamp_dealer_registration::~wamp_dealer_registration()
{
}

inline void wamp_dealer_registration::set_session(const std::shared_ptr<wamp_session>& session)
{
    m_session = session;
}

inline void wamp_dealer_registration::set_registration_id(const wamp_registration_id& registration_id)
{
    m_registration_id = registration_id;
}

inline const std::shared_ptr<wamp_session>& wamp_dealer_registration::get_session() const
{
    return m_session;
}

inline const wamp_registration_id& wamp_dealer_registration::get_registration_id() const
{
    return m_registration_id;
}

} // namespace bonefish

#endif // BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP
