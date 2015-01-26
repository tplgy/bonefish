#ifndef BONEFISH_MESSAGES_WAMP_PUBLISH_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_PUBLISH_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/wamp_uri.hpp>

namespace bonefish {

//
// [PUBLISH, Request|id, Options|dict, Topic|uri]
// [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list]
// [PUBLISH, Request|id, Options|dict, Topic|uri, Arguments|list, ArgumentsKw|dict]
//
class wamp_publish_message : public wamp_message
{
public:
    wamp_publish_message();
    virtual ~wamp_publish_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_uri& get_topic() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_topic(const wamp_uri& topic);

private:
    wamp_request_id m_request_id;
    wamp_uri m_topic;
};

inline wamp_publish_message::wamp_publish_message()
    : m_request_id()
    , m_topic()
{
}

inline wamp_publish_message::~wamp_publish_message()
{
}

inline wamp_message_type wamp_publish_message::get_type() const
{
    return wamp_message_type::PUBLISH;
}

inline const wamp_request_id& wamp_publish_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_uri& wamp_publish_message::get_topic() const
{
    return m_topic;
}

inline void wamp_publish_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_publish_message::set_topic(const wamp_uri& topic)
{
    m_topic = topic;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_PUBLISH_MESSAGE_HPP
