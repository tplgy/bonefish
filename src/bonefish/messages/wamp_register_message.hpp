#ifndef BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/utility/wamp_uri.hpp>
#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [REGISTER, Request|id, Options|dict, Procedure|uri]
//
class wamp_register_message : public wamp_message
{
public:
    wamp_register_message();
    virtual ~wamp_register_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    const msgpack::object& get_options() const;
    wamp_uri get_procedure() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_options(const msgpack::object& options);
    void set_procedure(const wamp_uri& procedure);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_options;
    msgpack::object m_procedure;

private:
    static const size_t NUM_FIELDS = 4;
};

inline wamp_register_message::wamp_register_message()
    : m_zone()
    , m_type(wamp_message_type::REGISTER, &m_zone)
    , m_request_id()
    , m_options(MSGPACK_EMPTY_MAP)
    , m_procedure()
{
}

inline wamp_register_message::~wamp_register_message()
{
}

inline wamp_message_type wamp_register_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_register_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id, m_options };
    return fields;
}

inline void wamp_register_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_options = msgpack::object(fields[2], &m_zone);
    m_procedure = msgpack::object(fields[3], &m_zone);
}

inline wamp_request_id wamp_register_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_register_message::get_options() const
{
    return m_options;
}

inline wamp_uri wamp_register_message::get_procedure() const
{
    return m_procedure.as<wamp_uri>();
}

inline void wamp_register_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_register_message::set_options(const msgpack::object& options)
{
    assert(options.type == msgpack::type::MAP);
    m_options = msgpack::object(options, &m_zone);
}

inline void wamp_register_message::set_procedure(const wamp_uri& procedure)
{
    m_procedure = msgpack::object(procedure, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
