#ifndef BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [UNREGISTERED, UNREGISTER.Request|id]
//
class wamp_unregistered_message : public wamp_message
{
public:
    wamp_unregistered_message();
    virtual ~wamp_unregistered_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    void set_request_id(const wamp_request_id& request_id);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;

private:
    static const size_t NUM_FIELDS = 2;
};

inline wamp_unregistered_message::wamp_unregistered_message()
    : m_zone()
    , m_type(wamp_message_type::UNREGISTERED)
    , m_request_id()
{
}

inline wamp_unregistered_message::~wamp_unregistered_message()
{
}

inline wamp_message_type wamp_unregistered_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_unregistered_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id };
    return fields;
}

inline void wamp_unregistered_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
}

inline wamp_request_id wamp_unregistered_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline void wamp_unregistered_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_UNREGISTERED_MESSAGE_HPP
