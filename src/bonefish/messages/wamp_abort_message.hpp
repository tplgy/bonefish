#ifndef BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cassert>
#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [ABORT, Details|dict, Reason|uri]
//
class wamp_abort_message : public wamp_message
{
public:
    wamp_abort_message();
    virtual ~wamp_abort_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    const msgpack::object& get_details() const;
    std::string get_reason() const;

    void set_details(const msgpack::object& details);
    void set_reason(const std::string& reason);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_details;
    msgpack::object m_reason;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_abort_message::wamp_abort_message()
    : m_zone()
    , m_type(wamp_message_type::ABORT)
    , m_details(msgpack_empty_map())
    , m_reason()
{
}

inline wamp_abort_message::~wamp_abort_message()
{
}

inline wamp_message_type wamp_abort_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_abort_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_details, m_reason };
    return fields;
}

inline void wamp_abort_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_details = msgpack::object(fields[1], &m_zone);
    m_reason = msgpack::object(fields[2], &m_zone);
}

inline const msgpack::object& wamp_abort_message::get_details() const
{
    return m_details;
}

inline std::string wamp_abort_message::get_reason() const
{
    return m_reason.as<std::string>();
}

inline void wamp_abort_message::set_details(const msgpack::object& details)
{
    assert(details.type == msgpack::type::MAP);
    m_details = msgpack::object(details, &m_zone);
}

inline void wamp_abort_message::set_reason(const std::string& reason)
{
    m_reason = msgpack::object(reason, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
