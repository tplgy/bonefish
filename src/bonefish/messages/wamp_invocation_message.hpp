#ifndef BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP

#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict]
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list]
// [INVOCATION, Request|id, REGISTERED.Registration|id, Details|dict, CALL.Arguments|list, CALL.ArgumentsKw|dict]
//
class wamp_invocation_message : public wamp_message
{
public:
    wamp_invocation_message();
    virtual ~wamp_invocation_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    wamp_registration_id get_registration_id() const;
    const msgpack::object& get_details() const;
    const msgpack::object& get_arguments() const;
    const msgpack::object& get_arguments_kw() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_registration_id(const wamp_registration_id& registration_id);
    void set_details(const msgpack::object& details);
    void set_arguments(const msgpack::object& arguments);
    void set_arguments_kw(const msgpack::object& arguments_kw);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_registration_id;
    msgpack::object m_details;
    msgpack::object m_arguments;
    msgpack::object m_arguments_kw;

private:
    static const size_t MIN_FIELDS = 4;
    static const size_t MAX_FIELDS = 6;
};

inline wamp_invocation_message::wamp_invocation_message()
    : m_zone()
    , m_type(wamp_message_type::INVOCATION, &m_zone)
    , m_request_id()
    , m_registration_id()
    , m_details(MSGPACK_EMPTY_MAP)
    , m_arguments()
    , m_arguments_kw()
{
}

inline wamp_invocation_message::~wamp_invocation_message()
{
}

inline wamp_message_type wamp_invocation_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_invocation_message::marshal() const
{
    std::vector<msgpack::object> fields;

    if (!m_arguments_kw.is_nil()) {
        if (!m_arguments.is_nil()) {
            fields = { m_type, m_request_id, m_registration_id, m_details,
                    m_arguments, m_arguments_kw };
        } else {
            fields = { m_type, m_request_id, m_registration_id, m_details,
                    MSGPACK_EMPTY_MAP, m_arguments_kw };
        }
    } else if (!m_arguments.is_nil()) {
        fields = { m_type, m_request_id, m_registration_id, m_details, m_arguments };
    } else {
        fields = { m_type, m_request_id, m_registration_id, m_details };
    }

    return fields;
}

inline void wamp_invocation_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() < MIN_FIELDS || fields.size() > MAX_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_registration_id = msgpack::object(fields[2]);
    m_details = msgpack::object(fields[3], &m_zone);
    if (fields.size() >= 5) {
        m_arguments = msgpack::object(fields[4], &m_zone);
    }
    if (fields.size() == 6) {
        m_arguments_kw = msgpack::object(fields[5], &m_zone);
    }
}

inline wamp_request_id wamp_invocation_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline wamp_registration_id wamp_invocation_message::get_registration_id() const
{
    return wamp_registration_id(m_registration_id.as<uint64_t>());
}

inline const msgpack::object& wamp_invocation_message::get_details() const
{
    return m_details;
}

inline const msgpack::object& wamp_invocation_message::get_arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_invocation_message::get_arguments_kw() const
{
    return m_arguments_kw;
}

inline void wamp_invocation_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_invocation_message::set_registration_id(const wamp_registration_id& registration_id)
{
    m_registration_id = msgpack::object(registration_id.id());
}

inline void wamp_invocation_message::set_details(const msgpack::object& details)
{
    if (details.type == msgpack::type::MAP) {
        m_details = msgpack::object(details, &m_zone);
    } else {
        throw std::invalid_argument("invalid details");
    }
}

inline void wamp_invocation_message::set_arguments(const msgpack::object& arguments)
{
    if (arguments.type == msgpack::type::NIL || arguments.type == msgpack::type::ARRAY) {
        m_arguments = msgpack::object(arguments, &m_zone);
    } else {
        throw std::invalid_argument("invalid arguments");
    }
}

inline void wamp_invocation_message::set_arguments_kw(const msgpack::object& arguments_kw)
{
    if (arguments_kw.type == msgpack::type::NIL || arguments_kw.type == msgpack::type::MAP) {
        m_arguments_kw = msgpack::object(arguments_kw, &m_zone);
    } else {
        throw std::invalid_argument("invalid arguments_kw");
    }
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_INVOCATION_MESSAGE_HPP
