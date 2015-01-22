#ifndef BONEFISH_MESSAGES_WAMP_UNREGISTER_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_UNREGISTER_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [UNREGISTER, Request|id, REGISTERED.Registration|id]
//
class wamp_unregister_message : public wamp_message
{
public:
    wamp_unregister_message();
    virtual ~wamp_unregister_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_registration_id& get_registration_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_registration_id(const wamp_registration_id& registration_id);

private:
    wamp_request_id m_request_id;
    wamp_registration_id m_registration_id;
};

inline wamp_unregister_message::wamp_unregister_message()
    : m_request_id()
{
}

inline wamp_unregister_message::~wamp_unregister_message()
{
}

inline wamp_message_type wamp_unregister_message::get_type() const
{
    return wamp_message_type::UNREGISTER;
}

inline const wamp_request_id& wamp_unregister_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_registration_id& wamp_unregister_message::get_registration_id() const
{
    return m_registration_id;
}

inline void wamp_unregister_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_unregister_message::set_registration_id(const wamp_registration_id& registration_id)
{
    m_registration_id = registration_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_UNREGISTER_MESSAGE_HPP
