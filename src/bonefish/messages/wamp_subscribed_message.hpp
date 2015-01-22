#ifndef BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/subscription_id.hpp>

namespace bonefish {

//
// [SUBSCRIBED, SUBSCRIBE.Request|id, Subscription|id]
//
class wamp_subscribed_message : public wamp_message
{
public:
    wamp_subscribed_message();
    virtual ~wamp_subscribed_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_subscription_id& get_subscription_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_subscription_id(const wamp_subscription_id& subscription_id);

private:
    wamp_request_id m_request_id;
    wamp_subscription_id m_subscription_id;
};

inline wamp_subscribed_message::wamp_subscribed_message()
    : m_request_id()
    , m_subscription_id()
{
}

inline wamp_subscribed_message::~wamp_subscribed_message()
{
}

inline wamp_message_type wamp_subscribed_message::get_type() const
{
    return wamp_message_type::SUBSCRIBED;
}

inline const wamp_request_id& wamp_subscribed_message::get_request_id() const
{
    return m_request_id;
}

inline const subscription_id& wamp_subscribed_message::get_subscription_id() const
{
    return m_subscription_id;
}

inline void wamp_subscribed_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_subscribed_message::set_subscription_id(const subscription_id& subscription_id)
{
    m_subscription_id = subscription_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP
