#ifndef BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

namespace bonefish {

//
// [PUBLISHED, PUBLISHED.Request|id, Publication|id]
//
class wamp_published_message : public wamp_message
{
public:
    wamp_published_message();
    virtual ~wamp_published_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_publication_id& get_publication_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_publication_id(const wamp_publication_id& publication_id);

private:
    wamp_request_id m_request_id;
    wamp_publication_id m_publication_id;
};

inline wamp_published_message::wamp_published_message()
    : m_request_id()
    , m_publication_id()
{
}

inline wamp_published_message::~wamp_published_message()
{
}

inline wamp_message_type wamp_published_message::get_type() const
{
    return wamp_message_type::PUBLISHED;
}

inline const wamp_request_id& wamp_published_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_publication_id& wamp_published_message::get_publication_id() const
{
    return m_publication_id;
}

inline void wamp_published_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_published_message::set_publication_id(const wamp_publication_id& publication_id)
{
    m_publication_id = publication_id;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP
