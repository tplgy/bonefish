#ifndef BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP

#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict]
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list]
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list, PUBLISH.ArgumentsKw|dict]
//
class wamp_event_message : public wamp_message
{
public:
    wamp_event_message();
    virtual ~wamp_event_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_subscription_id& get_subscription_id() const;
    const wamp_publication_id& get_publication_id() const;

    void set_subscription_id(const wamp_subscription_id& subscription_id);
    void set_publication_id(const wamp_publication_id& publication_id);

private:
    wamp_subscription_id m_subscription_id;
    wamp_publication_id m_publication_id;
};

inline wamp_event_message::wamp_event_message()
    : m_subscription_id()
    , m_publication_id()
{
}

inline wamp_event_message::~wamp_event_message()
{
}

inline wamp_message_type wamp_event_message::get_type() const
{
    return wamp_message_type::EVENT;
}

inline const wamp_subscription_id& wamp_event_message::get_subscription_id() const
{
    return m_subscription_id;
}

inline const wamp_publication_id& wamp_event_message::get_publication_id() const
{
    return m_publication_id;
}

inline void wamp_event_message::set_subscription_id(const wamp_subscription_id& subscription_id)
{
    m_subscription_id = subscription_id;
}

inline void wamp_event_message::set_publication_id(const wamp_publication_id& publication_id)
{
    m_publication_id = publication_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP
