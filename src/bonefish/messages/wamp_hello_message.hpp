#ifndef BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cassert>
#include <cstddef>
#include <iostream>
#include <msgpack.hpp>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace bonefish {

//
// [HELLO, Realm|uri, Details|dict]
//
class wamp_hello_message : public wamp_message
{
public:
    wamp_hello_message();
    virtual ~wamp_hello_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_uri get_realm() const;
    const msgpack::object& get_details() const;

    void set_realm(const wamp_uri& realm);
    void set_details(const msgpack::object& details);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_realm;
    msgpack::object m_details;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_hello_message::wamp_hello_message()
    : m_zone()
    , m_type(wamp_message_type::HELLO, &m_zone)
    , m_realm()
    , m_details(MSGPACK_EMPTY_MAP)
{
}

inline wamp_hello_message::~wamp_hello_message()
{
}

inline wamp_message_type wamp_hello_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_hello_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_realm, m_details };
    return fields;
}

inline void wamp_hello_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_realm = msgpack::object(fields[1], &m_zone);
    m_details = msgpack::object(fields[2], &m_zone);

    std::cerr << "hello message details: " << m_details << std::endl;
}

inline wamp_uri wamp_hello_message::get_realm() const
{
    return m_realm.as<wamp_uri>();
}

inline const msgpack::object& wamp_hello_message::get_details() const
{
    return m_details;
}

inline void wamp_hello_message::set_realm(const wamp_uri& realm)
{
    m_realm = msgpack::object(realm, &m_zone);
}

inline void wamp_hello_message::set_details(const msgpack::object& details)
{
    assert(details.type == msgpack::type::MAP);
    m_details = msgpack::object(details, &m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_HELLO_MESSAGE_HPP
