/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/roles/wamp_role.hpp>

#include <cassert>
#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [WELCOME, Session|id, Details|dict]
//
class wamp_welcome_message : public wamp_message
{
public:
    wamp_welcome_message();
    wamp_welcome_message(msgpack::zone&& zone);
    virtual ~wamp_welcome_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) override;

    wamp_session_id get_session_id() const;
    const msgpack::object& get_details() const;

    void set_session_id(const wamp_session_id& session_id);
    void set_details(const msgpack::object&);

private:
    msgpack::object m_type;
    msgpack::object m_session_id;
    msgpack::object m_details;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_welcome_message::wamp_welcome_message()
    : wamp_welcome_message(msgpack::zone())
{
}

inline wamp_welcome_message::wamp_welcome_message(msgpack::zone&& zone)
    : wamp_message(std::move(zone))
    , m_type(wamp_message_type::WELCOME)
    , m_session_id()
    , m_details(msgpack_empty_map())
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

inline void wamp_welcome_message::unmarshal(
        const std::vector<msgpack::object>& fields,
        msgpack::zone&& zone)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    acquire_zone(std::move(zone));
    m_session_id = fields[1];
    m_details = fields[2];
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
    m_details = msgpack::object(details, get_zone());
}

inline std::ostream& operator<<(std::ostream& os, const wamp_welcome_message& message)
{
    os << "welcome [" << message.get_session_id() << ", "
            << message.get_details() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_WELCOME_MESSAGE_HPP
