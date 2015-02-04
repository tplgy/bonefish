#ifndef BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

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
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    wamp_publication_id get_publication_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_publication_id(const wamp_publication_id& publication_id);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_publication_id;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_published_message::wamp_published_message()
    : m_zone()
    , m_type(wamp_message_type::PUBLISHED, &m_zone)
    , m_request_id()
    , m_publication_id()
{
}

inline wamp_published_message::~wamp_published_message()
{
}

inline wamp_message_type wamp_published_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_published_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id, m_publication_id };
    return fields;
}

inline void wamp_published_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_publication_id = msgpack::object(fields[2]);
}

inline wamp_request_id wamp_published_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline wamp_publication_id wamp_published_message::get_publication_id() const
{
    return wamp_publication_id(m_publication_id.as<uint64_t>());
}

inline void wamp_published_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id(), &m_zone);
}

inline void wamp_published_message::set_publication_id(const wamp_publication_id& publication_id)
{
    m_publication_id = msgpack::object(publication_id.id(), &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_PUBLISHED_MESSAGE_HPP
