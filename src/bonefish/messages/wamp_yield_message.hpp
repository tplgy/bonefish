#ifndef BONEFISH_MESSAGES_WAMP_YIELD_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_YIELD_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/subscription_id.hpp>

namespace bonefish {

//
// [YIELD, UNSUBSCRIBE.Request|id]
//
class wamp_yield_message : public wamp_message
{
public:
    wamp_yield_message();
    virtual ~wamp_yield_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    void set_request_id(const wamp_request_id& request_id);

private:
    wamp_request_id m_request_id;
};

inline wamp_yield_message::wamp_yield_message()
    : m_request_id()
{
}

inline wamp_yield_message::~wamp_yield_message()
{
}

inline wamp_message_type wamp_yield_message::get_type() const
{
    return wamp_message_type::YIELD;
}

inline const wamp_request_id& wamp_yield_message::get_request_id() const
{
    return m_request_id;
}

inline void wamp_yield_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_YIELD_MESSAGE_HPP
