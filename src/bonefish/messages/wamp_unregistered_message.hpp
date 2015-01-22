#ifndef BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [UNREGISTERED, UNREGISTER.Request|id]
//
class wamp_unregistered_message : public wamp_message
{
public:
    wamp_unregistered_message();
    virtual ~wamp_unregistered_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    void set_request_id(const wamp_request_id& request_id);

private:
    wamp_request_id m_request_id;
};

inline wamp_unregistered_message::wamp_unregistered_message()
    : m_request_id()
{
}

inline wamp_unregistered_message::~wamp_unregistered_message()
{
}

inline wamp_message_type wamp_unregistered_message::get_type() const
{
    return wamp_message_type::UNREGISTERED;
}

inline const wamp_request_id& wamp_unregistered_message::get_request_id() const
{
    return m_request_id;
}

inline void wamp_unregistered_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP
