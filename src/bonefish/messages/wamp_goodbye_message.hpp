#ifndef BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>
#include <cstddef>
#include <memory>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [GOODBYE, Details|dict, Reason|uri]
//
class wamp_goodbye_message : public wamp_message
{
public:
    wamp_goodbye_message();
    virtual ~wamp_goodbye_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    const msgpack::object& get_details() const;
    const wamp_uri get_reason() const;

    void set_details(const msgpack::object& details);
    void set_reason(const wamp_uri& reason);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_details;
    msgpack::object m_reason;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_goodbye_message::wamp_goodbye_message()
    : m_zone()
    , m_type(wamp_message_type::GOODBYE, &m_zone)
    , m_details(MSGPACK_EMPTY_MAP)
    , m_reason()
{
}

inline wamp_goodbye_message::~wamp_goodbye_message()
{
}

inline wamp_message_type wamp_goodbye_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_goodbye_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_details, m_reason };
    return fields;
}

inline void wamp_goodbye_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw(std::invalid_argument("invalid number of fields"));
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw(std::invalid_argument("invalid message type"));
    }

    m_details = msgpack::object(fields[1], &m_zone);
    m_reason = msgpack::object(fields[2], &m_zone);
}

inline const msgpack::object& wamp_goodbye_message::get_details() const
{
    return m_details;
}

inline const wamp_uri wamp_goodbye_message::get_reason() const
{
    return m_reason.as<wamp_uri>();
}

inline void wamp_goodbye_message::set_details(const msgpack::object& details)
{
    assert(details.type == msgpack::type::MAP);
    m_details = msgpack::object(details, &m_zone);
}

inline void wamp_goodbye_message::set_reason(const wamp_uri& reason)
{
    m_reason = msgpack::object(reason, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_GOODBYE_MESSAGE_HPP
