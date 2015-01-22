#ifndef BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict]
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list]
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list, CALL.ArgumentsKw|dict]
//
class wamp_invocation_message : public wamp_message
{
public:
    wamp_invocation_message();
    virtual ~wamp_invocation_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_registration_id& get_registration_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_registration_id(const wamp_registration_id& registration_id);

private:
    wamp_request_id m_request_id;
    wamp_registration_id m_registration_id;
};

inline wamp_invocation_message::wamp_invocation_message()
    : m_request_id()
{
}

inline wamp_invocation_message::~wamp_invocation_message()
{
}

inline wamp_message_type wamp_invocation_message::get_type() const
{
    return wamp_message_type::INVOCATION;
}

inline const wamp_request_id& wamp_invocation_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_registration_id& wamp_invocation_message::get_registration_id() const
{
    return m_registration_id;
}

inline void wamp_invocation_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_invocation_message::set_registration_id(const wamp_registration_id& registration_id)
{
    m_registration_id = registration_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP
