#ifndef BONEFISH_MESSAGES_WAMP_UNSUBSCRIBE_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_UNSUBSCRIBE_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [UNSUBSCRIBE, Request|id, SUBSCRIBED.Subscription|id]
//
class wamp_unsubscribe_message : public wamp_message
{
public:
    wamp_unsubscribe_message();
    virtual ~wamp_unsubscribe_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_subscription_id& get_subscription_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_subscription_id(const wamp_subscription_id& subscription_id);

private:
    wamp_request_id m_request_id;
    wamp_subscription_id m_subscription_id;
};

inline wamp_unsubscribe_message::wamp_unsubscribe_message()
    : m_request_id()
    , m_subscription_id()
{
}

inline wamp_unsubscribe_message::~wamp_unsubscribe_message()
{
}

inline wamp_message_type wamp_unsubscribe_message::get_type() const
{
    return wamp_message_type::UNSUBSCRIBE;
}

inline const wamp_request_id& wamp_unsubscribe_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_subscription_id& wamp_unsubscribe_message::get_subscription_id() const
{
    return m_subscription_id;
}

inline void wamp_unsubscribe_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_unsubscribe_message::set_subscription_id(const wamp_subscription_id& subscription_id)
{
    m_subscription_id = subscription_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_UNSUBSCRIBE_MESSAGE_HPP
