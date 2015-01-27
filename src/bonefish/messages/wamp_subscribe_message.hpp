#ifndef BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

namespace bonefish {

//
// [SUBSCRIBE, Request|id, Options|dict, Procedure|uri]
//
class wamp_subscribe_message : public wamp_message
{
public:
    wamp_subscribe_message();
    virtual ~wamp_subscribe_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_uri& get_topic() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_topic(const wamp_uri& topic);

private:
    wamp_request_id m_request_id;
    wamp_uri m_topic;
};

inline wamp_subscribe_message::wamp_subscribe_message()
    : m_request_id()
    , m_topic()
{
}

inline wamp_subscribe_message::~wamp_subscribe_message()
{
}

inline wamp_message_type wamp_subscribe_message::get_type() const
{
    return wamp_message_type::SUBSCRIBE;
}

inline const wamp_request_id& wamp_subscribe_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_uri& wamp_subscribe_message::get_topic() const
{
    return m_topic;
}

inline void wamp_subscribe_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_subscribe_message::set_topic(const wamp_uri& topic)
{
    m_topic = topic;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP
