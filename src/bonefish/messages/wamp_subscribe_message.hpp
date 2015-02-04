#ifndef BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [SUBSCRIBE, Request|id, Options|dict, Topic|uri]
//
class wamp_subscribe_message : public wamp_message
{
public:
    wamp_subscribe_message();
    virtual ~wamp_subscribe_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    const msgpack::object& get_options() const;
    wamp_uri get_topic() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_options(const msgpack::object& options);
    void set_topic(const wamp_uri& topic);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_options;
    msgpack::object m_topic;

private:
    static const size_t NUM_FIELDS = 4;
};

inline wamp_subscribe_message::wamp_subscribe_message()
    : m_zone()
    , m_type(wamp_message_type::SUBSCRIBE, &m_zone)
    , m_request_id()
    , m_options(MSGPACK_EMPTY_MAP)
    , m_topic()
{
}

inline wamp_subscribe_message::~wamp_subscribe_message()
{
}

inline wamp_message_type wamp_subscribe_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_subscribe_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id, m_options, m_topic };
    return fields;
}

inline void wamp_subscribe_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_options = msgpack::object(fields[2], &m_zone);
    m_topic = msgpack::object(fields[3], &m_zone);
}

inline wamp_request_id wamp_subscribe_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_subscribe_message::get_options() const
{
    return m_options;
}

inline wamp_uri wamp_subscribe_message::get_topic() const
{
    return m_topic.as<wamp_uri>();
}

inline void wamp_subscribe_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_subscribe_message::set_options(const msgpack::object& options)
{
    assert(options.type == msgpack::type::MAP);
    m_options = msgpack::object(options, &m_zone);
}

inline void wamp_subscribe_message::set_topic(const wamp_uri& topic)
{
    m_topic = msgpack::object(topic, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_SUBSCRIBE_MESSAGE_HPP
