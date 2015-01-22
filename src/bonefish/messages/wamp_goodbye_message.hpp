#ifndef BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/wamp_uri.hpp>

namespace bonefish {

//
// [GOODBYE, Details|dict, Reason|uri]
//
class wamp_goodbye_message : public wamp_message
{
public:
    wamp_goodbye_message();
    virtual ~wamp_goodbye_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_uri& get_reason() const;
    void set_reason(const wamp_uri& reason);

private:
    wamp_uri m_reason;
};

inline wamp_goodbye_message::wamp_goodbye_message()
    : m_reason()
{
}

inline wamp_goodbye_message::~wamp_goodbye_message()
{
}

inline wamp_message_type wamp_goodbye_message::get_type() const
{
    return wamp_message_type::GOODBYE;
}

inline const wamp_uri& wamp_goodbye_message::get_reason() const
{
    return m_reason;
}

inline void wamp_goodbye_message::set_reason(const wamp_uri& reason)
{
    m_reason = reason;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP
