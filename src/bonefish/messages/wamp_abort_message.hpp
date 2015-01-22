#ifndef BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/wamp_uri.hpp>

namespace bonefish {

//
// [ABORT, Details|dict, Reason|uri]
//
class wamp_abort_message : public wamp_message
{
public:
    wamp_abort_message();
    virtual ~wamp_abort_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_uri& get_reason() const;
    void set_reason(const wamp_uri& reason);

private:
    wamp_uri m_reason;
};

inline wamp_abort_message::wamp_abort_message()
    : m_reason()
{
}

inline wamp_abort_message::~wamp_abort_message()
{
}

inline wamp_message_type wamp_abort_message::get_type() const
{
    return wamp_message_type::ABORT;
}

inline const wamp_uri& wamp_abort_message::get_reason() const
{
    return m_reason;
}

inline void wamp_abort_message::set_reason(const wamp_uri& reason)
{
    m_reason = reason;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
