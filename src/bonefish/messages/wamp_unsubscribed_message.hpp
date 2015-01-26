#ifndef BONEFISH_MESSAGES_WAMP_UNSUBSCRIBED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_UNSUBSCRIBED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [UNSUBSCRIBED, UNSUBSCRIBE.Request|id]
//
class wamp_unsubscribed_message : public wamp_message
{
public:
    wamp_unsubscribed_message();
    virtual ~wamp_unsubscribed_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    void set_request_id(const wamp_request_id& request_id);

private:
    wamp_request_id m_request_id;
};

inline wamp_unsubscribed_message::wamp_unsubscribed_message()
    : m_request_id()
{
}

inline wamp_unsubscribed_message::~wamp_unsubscribed_message()
{
}

inline wamp_message_type wamp_unsubscribed_message::get_type() const
{
    return wamp_message_type::UNSUBSCRIBED;
}

inline const wamp_request_id& wamp_unsubscribed_message::get_request_id() const
{
    return m_request_id;
}

inline void wamp_unsubscribed_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_UNSUBSCRIBED_MESSAGE_HPP
