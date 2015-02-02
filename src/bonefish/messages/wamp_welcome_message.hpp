#ifndef BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <cstddef>
#include <memory>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>
#include <unordered_set>

namespace bonefish {

//
// [WELCOME, Session|id, Details|dict]
//
class wamp_welcome_message : public wamp_message
{
public:
    wamp_welcome_message();
    virtual ~wamp_welcome_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_session_id get_session_id() const;
    const msgpack::object& get_details() const;

    void set_session_id(const wamp_session_id& session_id);
    void set_details(const msgpack::object&);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_session_id;
    msgpack::object m_details;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_welcome_message::wamp_welcome_message()
    : m_zone()
    , m_type(wamp_message_type::WELCOME, &m_zone)
    , m_session_id()
    , m_details(MSGPACK_EMPTY_MAP)
{
}

inline wamp_welcome_message::~wamp_welcome_message()
{
}

inline wamp_message_type wamp_welcome_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_welcome_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_session_id, m_details };
    return fields;
}

inline void wamp_welcome_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw(std::invalid_argument("invalid number of fields"));
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw(std::invalid_argument("invalid message type"));
    }

    m_session_id = msgpack::object(fields[1]);
    m_details = msgpack::object(fields[2], &m_zone);
}

inline wamp_session_id wamp_welcome_message::get_session_id() const
{
    return wamp_session_id(m_session_id.as<uint64_t>());
}

inline const msgpack::object& wamp_welcome_message::get_details() const
{
    return m_details;
}

inline void wamp_welcome_message::set_session_id(const wamp_session_id& session_id)
{
    m_session_id = msgpack::object(session_id.id());
}

inline void wamp_welcome_message::set_details(const msgpack::object& details)
{
    assert(details.type == msgpack::type::MAP);
    m_details = msgpack::object(details, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP
